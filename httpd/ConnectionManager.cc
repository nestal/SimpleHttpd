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
#include <iostream>

namespace http {

using namespace boost::asio;

class ConnectionManager::Entry :
	public Request,
	public RequestCallback,
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
	void OnMessageStart(http::Method method, std::string&& url, int, int) override
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
		m_content_handler = m_handler.HandleRequest(*this);
		if (!m_content_handler)
		{
			DoReply({HTTP_STATUS_BAD_REQUEST});
			return -1;
		}
		else
		{
			return 0;
		}
	}
	
	int OnContent(const char *data, std::size_t size) override
	{
		assert(m_content_handler);
		auto response = m_content_handler->OnContent(data, size);
		
		// pre-mature response indicates an error from the content handler
		if (response.valid())
		{
			TryReply(std::move(response));
			return -1;
		}
		else
		{
			return 0;
		}
	}
	int OnMessageEnd() override
	{
		assert(m_content_handler);
		auto response = m_content_handler->Finish();
		assert(response.valid());
		TryReply(std::move(response));
		return 0;
	}
	
	void Read();
	void Stop();
	void OnRead(std::size_t count)
	{
		m_parser.Parse(m_read_buffer.begin(), count);
		if (m_parser.CurrentProgress() != HTTPParser::Progress::finished)
			Read();
	}
	
	void TryReply(future<Response> response)
	{
		assert(response.valid());
		response.then([this, self=shared_from_this()](auto fut_reponse)
		{
			DoReply(fut_reponse.get());
		}, use_service<Executor>(IoService()));
	}
	
	void DoReply(Response response)
	{
		response.Send(m_socket).then([this, self=shared_from_this()](auto fut_ec)
		{
			auto ec = fut_ec.get();
			if (!ec)
			{
				std::cout << "finished connection" << std::endl;
				
				// Initiate graceful connection closure.
				boost::system::error_code ignored_ec;
				m_socket.shutdown(ip::tcp::socket::shutdown_both, ignored_ec);
			}
			
			if (ec != boost::asio::error::operation_aborted)
				m_parent.Stop(shared_from_this());
		}, use_service<Executor>(IoService()));
	}
	
	std::string URL() const override
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

	http::Method    m_method;
	std::string     m_uri;
	HeaderList      m_headers;

	ConnectionManager&  m_parent;
	HTTPParser          m_parser{*this};
	const RequestDispatcher& m_handler;
	
	std::unique_ptr<ContentHandler> m_content_handler;
};

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
