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

#include "HTTPParser.hh"
#include "Response.hh"
#include "HeaderList.hh"

#include "executor/BoostAsioExecutor.hh"

#include <cassert>

namespace http {

class ConnectionManager::Entry :
	public Request,
	public HTTPParser::Callback,
	public std::enable_shared_from_this<Entry>
{
public:
	Entry(
		boost::asio::ip::tcp::socket&&  sock,
		const RequestDispatcher&        handler,
		ConnectionManager&              parent
	) : m_socket{std::move(sock)},
		m_parent{parent},
		m_handler{handler}
	{
	}
	
	// RequestCallback overrides
	void OnMessageStart(http::Method method, Status, std::string&& url) override
	{
		m_method = method;
		m_uri    = std::move(url);
	}
	void OnHeader(std::string&& field, std::string&& value) override
	{
		m_headers.Add({std::move(field), std::move(value)});
	}
	int OnHeaderComplete() override
	{
		auto&& response = m_handler.HandleRequest(*this);
		if (!m_content_handler)
			SendReply(std::move(response));
		return 0;
	}
	
	int OnContent(const char *data, std::size_t size) override
	{
		if (m_content_handler)
		{
			auto&& opt_response = m_content_handler->OnContent(*this, data, size);
			if (opt_response)
			{
				SendReply(std::move(*opt_response));
				m_content_handler.reset();
			}
		}
		return 0;
	}
	int OnMessageEnd() override
	{
		if (m_content_handler)
		{
			auto&& opt_response = m_content_handler->ReplyNow(*this);
			if (opt_response)
				SendReply(std::move(*opt_response));
			else
				Reply(m_content_handler->ReplyLater(*this));
		}
		
		return 0;
	}
	
	http::Executor& Executor() override
	{
		return boost::asio::use_service<http::Executor>(IoService());
	}
	
	void HandleContent(ContentHandlerPtr&& handler)
	{
		m_content_handler = std::move(handler);
	}
	
	void Read()
	{
		m_socket.async_read_some(
			boost::asio::buffer(m_read_buffer),
			[this, self=shared_from_this()](auto error_code, std::size_t count)
			{
				if (!error_code)
					OnRead(count);
	
				else if (error_code != boost::asio::error::operation_aborted)
					m_parent.Stop(shared_from_this());
			}
		);
	}
	
	void Stop()
	{
		m_socket.close();
	}
	
	void OnRead(std::size_t count)
	{
		m_parser.Parse(m_read_buffer.begin(), count);
		auto result = m_parser.Result();
		if (result != HPE_OK && result != HPE_CB_headers_complete && result != HPE_CB_body)
			SendReply(status_BAD_REQUEST);
			
		else if (m_parser.CurrentProgress() != HTTPParser::Progress::finished)
			Read();
	}
	
	void Reply(future<Response> response)
	{
		assert(response.valid());
		response.then([this, self=shared_from_this()](auto fut_reponse)
		{
			SendReply(fut_reponse.get());
		}, Executor());
	}
	
	void SendReply(Response&& response)
	{
		// keep the response from being destroyed when the async write is in-flight
		m_response = std::move(response);
		
		assert(!m_sent);
		m_response.Send(m_socket, [this, self=shared_from_this()](auto ec, auto)
		{
			if (!ec)
			{
				// Initiate graceful connection closure.
				boost::system::error_code ignored_ec;
				m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
			}
			
			if (ec != boost::asio::error::operation_aborted)
				m_parent.Stop(self);
		});
		m_sent = true;
	}
	
	boost::string_view URL() const override
	{
		return m_uri;
	}
	
	const HeaderList& Header() const override
	{
		return m_headers;
	}
	
	http::Method Method() const override
	{
		return m_method;
	}
	
	boost::asio::io_service& IoService() override
	{
		return m_socket.get_io_service();
	}
	
	boost::asio::ip::tcp::endpoint Client() const override
	{
		return m_socket.remote_endpoint();
	}
	
	boost::asio::ip::tcp::endpoint Local() const override
	{
		return m_socket.local_endpoint();
	}

private:
	boost::asio::ip::tcp::socket m_socket;
	std::array<char, 1024> m_read_buffer;

	bool            m_sent{false};
	http::Method    m_method;
	std::string     m_uri;
	HeaderList      m_headers;
	Response        m_response;

	ConnectionManager&  m_parent;
	HTTPParser          m_parser{*this};
	const RequestDispatcher& m_handler;
	
	std::unique_ptr<ContentHandler> m_content_handler;
};

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
