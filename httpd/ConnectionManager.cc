/*
	Copyright © 2016 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SongBits
    distribution for more details.
*/


//
// Created by nestal on 5/11/16.
//

#include "ConnectionManager.hh"

#include "RequestDispatcher.hh"

#include "Request.hh"
#include "HttpParser.hh"
#include "Response.hh"

#include "executor/BoostAsioExecutor.hh"

#include <cassert>

namespace http {

using namespace boost::asio;

class ConnectionManager::Entry : public Connection, public std::enable_shared_from_this<Entry>
{
public:
	Entry(
		boost::asio::ip::tcp::socket&&  sock,
		const RequestDispatcher&        handler,
		ConnectionManager&              parent
	);
	
	void Read();
	void Stop();
	
	const http::Request&  Request() override {return m_req;}
	void Reply(const Response& rep);
	io_service& IoService() override
	{
		return m_socket.get_io_service();
	}
	
	ip::tcp::endpoint Client() const override
	{
		return m_socket.remote_endpoint();
	}
	
	ip::tcp::endpoint Local() const override
	{
		return m_socket.local_endpoint();
	}

private:
	boost::asio::ip::tcp::socket m_socket;
	std::array<char, 1024> m_read_buffer;
	
	ConnectionManager&  m_parent;
	http::Request       m_req;
	HttpParser          m_parser;
	const RequestDispatcher& m_handler;
};

ConnectionManager::Entry::Entry(
	ip::tcp::socket&&         sock,
	const RequestDispatcher&  handler,
	ConnectionManager&        parent
) :
	m_socket{std::move(sock)},
	m_parent{parent},
	m_handler{handler}
{
	m_parser.SetCallback(m_req);
}

void ConnectionManager::Entry::Read()
{
	auto& exec = use_service<BrightFuture::BoostAsioExecutor>(m_socket.get_io_service());
	m_socket.async_read_some(buffer(m_read_buffer),
		[this, &exec, self=shared_from_this()](boost::system::error_code error, std::size_t count)
		{
			if (!error)
			{
				m_parser.Parse(m_read_buffer.begin(), count);
				if (m_parser.CurrentProgress() == HttpParser::Progress::finished)
				{
					if (m_parser.Result() == HPE_OK)
					{
						m_handler.HandleRequest(self).then([this, self](auto fut_reply)
						{
							Reply(fut_reply.get());
						}, exec);
					}
					else
					Reply({ResponseStatus::bad_request});
				}
				else
					Read();
			}

			else if (error != boost::asio::error::operation_aborted)
				m_parent.Stop(shared_from_this());
		});
}


void ConnectionManager::Entry::Reply(const Response& rep)
{
	async_write(m_socket, rep.ToBuffers(),
		[this, self=shared_from_this()](boost::system::error_code ec, std::size_t c)
		{
			if (!ec)
			{
				// Initiate graceful connection closure.
				boost::system::error_code ignored_ec;
				m_socket.shutdown(ip::tcp::socket::shutdown_both, ignored_ec);
			}
			
			if (ec != boost::asio::error::operation_aborted)
				m_parent.Stop(shared_from_this());
		});
}

void ConnectionManager::Entry::Stop()
{
	m_socket.close();
}

void ConnectionManager::Start(
	boost::asio::ip::tcp::socket&&  sock,
	const RequestDispatcher&        handler
)
{
	auto p = std::make_shared<ConnectionManager::Entry>(std::move(sock), handler, *this);

	m_conn.insert(p);
	p->Read();
}

void ConnectionManager::Stop(const EntryPtr& p)
{
	assert(p);
	assert(m_conn.find(p) != m_conn.end());

	m_conn.erase(p);
	p->Stop();
}

void ConnectionManager::StopAll()
{
	for (auto&& c : m_conn)
		c->Stop();
}

} // end of namespace
