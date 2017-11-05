/*
	Copyright © 2016 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SongBits
    distribution for more details.
*/

//
// Created by nestal on 5/11/16.
//

#pragma once

#include "ConnectionManager.hh"
#include "RequestHandler.hh"
#include "RequestDispatcher.hh"

#include <boost/asio.hpp>
#include <string>

/**
 * \brief Boost Asio-based HTTP server library
 *
 * The HTTP library is a callback-base asynchronous library. Users of the
 * library first constructs a Server object and register callbacks to it.
 * The callback will be run when the server receives a corresponding request.
 *
 * Most library functions are asynchronous. They use the io_service object
 * passed to the Server.
 */
namespace http {

/**
 * \brief HTTP Server Class
 *
 *
 */
class Server
{
private:
	template <typename Callable>
	class AdaptFutureResponse : public ContentHandler
	{
	public:
		AdaptFutureResponse(const Callable& h, Request& conn) : m_callable{h}, m_conn{conn} {}
		
		future<Response> OnContent(const char *, std::size_t) override {return {};}
		future<Response> Finish() override {return DoFinish();}
		
	private:
		// This overload of DoFinish() will only be enabled if the return value of the "Callable"
		// functor is a future<Response>.
		template <typename C=Callable>
		typename std::enable_if<
			std::is_same<
				typename std::result_of<C(Request&)>::type,
				future<Response>
			>::value,
			future<Response>
		>::type DoFinish() {return m_callable(std::move(m_conn));}
		
		// This overload of DoFinish() will only be enabled if the return value of the "Callable"
		// functor is a Response.
		template <typename C=Callable>
		typename std::enable_if<
			std::is_same<
				typename std::result_of<C(Request&)>::type,
				Response
			>::value,
			future<Response>
		>::type	DoFinish()
		{
			promise<Response> p;
			p.set_value(m_callable(std::move(m_conn)));
			return p.get_future();
		}
		
	private:
		const Callable& m_callable;
		Request&        m_conn;
	};

	template <typename Callable>
	typename std::enable_if<
		!std::is_convertible<
			typename std::result_of<Callable(Request&)>::type,
			ContentHandlerPtr
		>::value,
		RequestHandler
	>::type AdaptToRequestHandler(Callable&& handler)
	{
		return [handler=std::forward<Callable>(handler)](Request& conn)
		{
			return std::make_unique<AdaptFutureResponse<Callable>>(handler, conn);
		};
	}
	
	// This overload of Adapt() will only be enabled if the return value of the "Callable"
	// functor is convertible to ContentHandlerPtr.
	template <typename Callable>
	typename std::enable_if<
		std::is_convertible<
			typename std::result_of<Callable(Request&)>::type,
			ContentHandlerPtr
		>::value,
		RequestHandler
	>::type AdaptToRequestHandler(Callable&& handler)
	{
		return std::forward<Callable>(handler);
	}
	
public:
	Server() = delete;
	Server(const Server&) = delete;
	Server& operator=(const Server&) = delete;
	Server(Server&&) = default;
	Server& operator=(Server&&) = default;

	Server(
		boost::asio::io_service& ios,
		const std::string& address,
		const std::string& port
	);

	template <typename Callable>
	void AddHandler(const std::string& uri, Callable&& handler)
	{
		m_handlers.Add(uri, AdaptToRequestHandler(std::forward<Callable>(handler)));
	}
	
	template <typename Callable>
	void SetDefaultHandler(Callable&& handler)
	{
		m_handlers.SetDefault(AdaptToRequestHandler(std::forward<Callable>(handler)));
	}
	
	boost::asio::io_service& IoService();
	
private:
	/// Perform an asynchronous accept operation.
	void AsyncAccept();

	/// Wait for a request to stop the server.
	void DoAwaitStop();

private:
	/// The signal_set is used to register for process termination notifications.
	boost::asio::signal_set m_signals;

	/// Acceptor used to listen for incoming connections.
	boost::asio::ip::tcp::acceptor m_acceptor;

	/// The next socket to be accepted.
	boost::asio::ip::tcp::socket m_socket;

	ConnectionManager       m_conn;

	RequestDispatcher       m_handlers;
};

} // end of namespace
