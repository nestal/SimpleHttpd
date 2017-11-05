/*
	Copyright © 2016 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SongBits
    distribution for more details.
*/

//
// Created by nestal on 5/11/16.
//

#include "Server.hh"

namespace http {

using namespace boost::asio;

/**
 * \brief Construct the server to listen on the specified TCP address and port
 *
 * The constructor will listen and start accepting connections from the address
 * and port provided. The accepted connection will be asynchronously handled
 * in the io_service::run() specified in the argument.
 *
 * \param   ios     The io_service that runs all the asynchronous functions. It
 *                  will be returned by IoService().
 * \param   address Local IP address or host name to bind and listen to. It will
 *                  be resolved with the \a port by boost::asio::tcp::resolver.
 * \param   port    Local port number or service number.
 * \throw   boost::system_error
 */
Server::Server(io_service& ios, const std::string& address, const std::string& port) :
	m_signals{ios},
	m_acceptor{ios},
	m_socket{ios}
{
	DoAwaitStop();

	using namespace ip;
	
	// Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
	tcp::resolver resolver{ios};
	tcp::endpoint endpoint = *resolver.resolve({address, port});
	m_acceptor.open(endpoint.protocol());
	m_acceptor.set_option(tcp::acceptor::reuse_address(true));
	m_acceptor.bind(endpoint);
	m_acceptor.listen();
	
	AsyncAccept();
}

void Server::AsyncAccept()
{
	m_acceptor.async_accept(m_socket, [this](boost::system::error_code ec)
	{
		// Check whether the server was stopped by a signal before this
		// completion handler had a chance to run.
		if (m_acceptor.is_open())
		{
			if (!ec)
				m_conn.Start(std::move(m_socket), m_handlers);
			
			// wait for another connection to come
			AsyncAccept();
		}
	});
}

void Server::DoAwaitStop()
{
	m_signals.async_wait(
		[this](boost::system::error_code, int)
		{
			// The server is stopped by cancelling all outstanding asynchronous
			// operations. Once all operations have finished the io_service::run()
			// call will exit.
			m_acceptor.close();
			m_conn.StopAll();
		});
}

/**
 * \brief Get the io_service that runs this server.
 *
 * This is the same io_service that is passed in the constructor.
 *
 * \return The io_service passed in the constructor.
 */
io_service& Server::IoService()
{
	return m_acceptor.get_io_service();
}
	
} // end of namespace
