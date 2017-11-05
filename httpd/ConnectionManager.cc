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
#include "HTTPParser.hh"
#include "Response.hh"

#include "executor/BoostAsioExecutor.hh"

#include <cassert>

namespace http {

using namespace boost::asio;

class ConnectionManager::Entry :
	public Connection,
	public RequestCallback,
	public std::enable_shared_from_this<Entry>
{
public:
	Entry(
		boost::asio::ip::tcp::socket&&  sock,
		const RequestDispatcher&        handler,
		ConnectionManager&              parent
	);
	
	// RequestCallback overrides
	void OnMessageStart(http::Method method, std::string&& url, int major, int minor) override
	{
		m_req.OnMessageStart(method, std::move(url), major, minor);
	}
	void OnHeader(std::string&& field, std::string&& value) override
	{
		m_req.OnHeader(std::move(field), std::move(value));
	}
	void OnHeaderComplete() override
	{
		auto self = shared_from_this();
		m_handler.HandleRequest(self).then([this, self](auto fut_reply)
		{
			Reply(fut_reply.get());
		}, use_service<BrightFuture::BoostAsioExecutor>(m_socket.get_io_service()));
	}
	
	void OnContent(const char *data, std::size_t size) override
	{
		if (m_content)
			m_content->OnContent(data, size);
	}
	void OnMessageEnd() override
	{
		if (m_content)
			m_content->Finish();
	}
	void HandleContent(std::unique_ptr<ContentHandler> handler) override
	{
		m_content = std::move(handler);
	}
	
	void Read();
	void Stop();
	void OnRead(std::size_t count);
	void Reply(const Response& rep);
	
	const http::Request&  Request() override {return m_req;}
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
	HTTPParser          m_parser;
	const RequestDispatcher& m_handler;
	std::unique_ptr<ContentHandler> m_content;
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
	m_parser.SetCallback(*this);
}

void ConnectionManager::Entry::Read()
{
	m_socket.async_read_some(
		buffer(m_read_buffer),
		[this, self=shared_from_this()](auto error_code, std::size_t count)
		{
			if (!error_code)
				OnRead(count);

			else if (error_code != boost::asio::error::operation_aborted)
				m_parent.Stop(shared_from_this());
		}
	);
}

void ConnectionManager::Entry::OnRead(std::size_t count)
{
	try
	{
		m_parser.Parse(m_read_buffer.begin(), count);
		if (m_parser.CurrentProgress() != HTTPParser::Progress::finished)
			Read();
	}
	catch (std::exception& e)
	{
		Reply({ResponseStatus::bad_request});
	}
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
