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
#include "RequestParser.hh"
#include "Response.hh"

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
	
	void Read(BrightFuture::Executor *exec);
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
	RequestParser       m_parser;
	const RequestDispatcher& m_handler;
};

ConnectionManager::Entry::Entry(
	ip::tcp::socket&&         sock,
	const RequestDispatcher&  handler,
	ConnectionManager&        parent
) :
	m_socket(std::move(sock)),
	m_parent(parent),
	m_handler(handler)
{
}

void ConnectionManager::Entry::Read(BrightFuture::Executor *exec)
{
	assert(exec);
	
	m_socket.async_read_some(buffer(m_read_buffer),
		[this, exec, self=shared_from_this()](boost::system::error_code error, std::size_t count)
		{
			if (!error)
			{
				RequestParser::Result result;
				std::tie(result, std::ignore) = m_parser.Parse(
					m_req, m_read_buffer.begin(), m_read_buffer.begin() + count);
				
				if (result == RequestParser::good)
					m_handler.HandleRequest(self).then([this, self](auto fut_reply)
					{
						Reply(fut_reply.get());
					}, exec);
				
				else if (result == RequestParser::bad)
					Reply({ResponseStatus::bad_request});

				else
					Read(exec);
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

ConnectionManager::ConnectionManager(BrightFuture::Executor *exec) : m_exec{exec}
{
}

void ConnectionManager::Start(
	boost::asio::ip::tcp::socket&&  sock,
	const RequestDispatcher&        handler
)
{
	auto p = std::make_shared<ConnectionManager::Entry>(std::move(sock), handler, *this);

	m_conn.insert(p);
	p->Read(m_exec);
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
