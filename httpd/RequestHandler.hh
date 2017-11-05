/*
	Copyright © 2016 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SongBits
    distribution for more details.
*/

//
// Created by nestal on 5/15/16.
//

#pragma once

#include "Future.hh"

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <functional>
#include <memory>
#include <cstdint>

namespace http {

class Request;
class Response;
class HeaderList;
class Method;

class ContentHandler
{
public:
	virtual ~ContentHandler() = default;
	virtual future<http::Response> OnContent(const char *data, std::size_t size) = 0;
	virtual future<http::Response> Finish() = 0;
};
using ContentHandlerPtr = std::unique_ptr<ContentHandler>;

/**
 * \brief Represents the information required to handle an HTTP request
 *
 * The purpose of the Connection is to provide enough information for
 * the ::RequestHandler to handle a request. These information include
 * the HTTP request itself, i.e. the request headers and content, represented
 * by the Request class. It also include the response builder: the Response
 * class.
 */
class Connection
{
public:
	virtual std::string URL() const = 0;
	virtual const HeaderList& Header() const = 0;
	virtual http::Method Method() const = 0;

	/// \brief Returns the io_service that runs the Server
	///
	/// It is provided for convenient. It is typically implemented
	/// by calling get_io_service() on the socket. Ultimately this
	/// is the same io_service you passed to Server's constructor.
	///
	/// \return  The io_service that runs the Server
	/// \sa Server::IoService()
	virtual boost::asio::io_service& IoService() = 0;
	virtual boost::asio::ip::tcp::endpoint Client() const = 0;
	virtual boost::asio::ip::tcp::endpoint Local() const = 0;
};

/**
 * \brief shared_ptr to Connection
 *
 * Shared points of Connection are used so frequently that they deserve
 * a shorthand. Typically the implementation of Connection interface
 * will use std::enable_shared_from_this.
 */
using ConnectionPtr = std::shared_ptr<Connection>;

/**
 * \brief A function that handles HTTP requests.
 *
 * The RequestHandler is the basic unit of the web server. With a
 * RequestDispatcher, you can add RequestHandler's to handle a specific
 * set of URIs.
 *
 * These functions are normally implemented in classes that are
 * completely independent of RequestDispatcher or Server. That reduces
 * the coupling between the web server library and business logic. You
 * can easier test the #RequestHandler's without mocking any web server
 * objects.
 *
 * Typically you will pass lambda functions to places that expects
 * #RequestHandler's.
 *
 * You don't need to call Connection::Reply() within the handler function.
 * You can also post another callback to the IoService() and reply there.
 * Typically, you can issue another asynchronous operation, and call
 * Connection::Reply() in the completion callback. In that case, make sure
 * to capture the shared_ptr of Connection (i.e. #ConnectionPtr) _by value_
 * in the lambda callback function. Otherwise the Connection objects will
 * be destroyed if the connection is closed by peer or timed out.
 */
using RequestHandler = std::function<ContentHandlerPtr(const ConnectionPtr&)>;

} // end of namespace
