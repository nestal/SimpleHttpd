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
#include "Adaptors.hh"

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
		m_handlers.Add(uri, MakeHandler(std::forward<Callable>(handler)));
	}
	
	template <typename Callable>
	void SetDefaultHandler(Callable&& handler)
	{
		m_handlers.SetDefault(MakeHandler(std::forward<Callable>(handler)));
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
