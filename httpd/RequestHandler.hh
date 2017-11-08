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
#include "Response.hh"

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <memory>

namespace http {

class Request;
class Response;
class HeaderList;
class Method;

///
/// Abstract interface for handling HTTP request contents.
///
/// When you inherit this class and return it from a RequestHandler, the Server will call it to
/// handle the content of the HTTP request. The data in the HTTP request content will be passed
/// to OnContent(). After all HTTP request content is received, OnFinish() will be called.
///
/// Both OnContent() and Finish() return a future to Response, which will be sent to the client.
/// If you return a Response in OnContent(), the server will send the response immediately such
/// that further data in the request content will be ignored. That means OnContent() and Finish()
/// will not be called. If you want to carry on the content processing, return an invalid future
/// (i.e. default constructed future<Response>, or just "return {};")
///
class ContentHandler
{
public:
	virtual ~ContentHandler() = default;
	virtual future<Response> OnContent(Request& request, const char *data, std::size_t size) = 0;
	virtual future<Response> Finish(Request& request) = 0;
};
using ContentHandlerPtr = std::unique_ptr<ContentHandler>;

ContentHandlerPtr ResponseWith(const Response& response);
ContentHandlerPtr ResponseWith(Response&& response);
ContentHandlerPtr SaveContentToFile(const std::string& path,
	const Response& success = Response{HTTP_STATUS_OK},
	const Response& failed  = Response{HTTP_STATUS_INTERNAL_SERVER_ERROR}
);

///
/// Represents the information required to handle an HTTP request
///
/// The purpose of this class is to provide enough information for
/// the RequestHandler to handle a request. These information include
/// the HTTP request itself, i.e. the request headers and content, represented
/// by the Request class. It also include the response builder: the Response
/// class.
///
class Request
{
public:
	virtual std::string URL() const = 0;
	virtual const HeaderList& Header() const = 0;
	virtual http::Method Method() const = 0;

	virtual boost::asio::ip::tcp::endpoint Client() const = 0;
	virtual boost::asio::ip::tcp::endpoint Local() const = 0;

	/// \brief Returns the io_service that runs the Server
	///
	/// It is provided for convenient. It is typically implemented
	/// by calling get_io_service() on the socket. Ultimately this
	/// is the same io_service you passed to Server's constructor.
	///
	/// \return  The io_service that runs the Server
	/// \sa Server::IoService()
	virtual boost::asio::io_service& IoService() = 0;
	
	virtual http::Executor& Executor() = 0;
};

///
/// A function that handles HTTP requests.
///
/// The RequestHandler is the basic unit of the web server. With a
/// RequestDispatcher, you can add RequestHandler's to handle a specific
/// set of URIs by Server::AddHandler() and Server::SetDefaultHandler().
///
/// These functions are normally implemented in classes that are
/// completely independent of RequestDispatcher or Server. That reduces
/// the coupling between the web server library and business logic. You
/// can easily test the #RequestHandler's without mocking any web server
/// objects. Typically you will pass lambda functions to places that expects
/// #RequestHandler's.
///
/// The RequestHandler will be called when the server has received all
/// headers of the HTTP request. It should return a unique_ptr to a
/// ContentHandler that will process the upcoming content of the
/// request. The ContentHandler will also provide a Response that will
/// be replied to the HTTP client. See ContentHandler for more details.
///
/// In some cases you want to ignore the Request content and reply a
/// Response immediately, you can use AdaptToRequestHandler to create
/// a ContentHandler with a callback function that returns a Response
/// or future<Response>.
///
/// \param Request&     A reference to the Request that is being handled
/// by this function. The reference will always be valid until the
/// connection to client is closed. Typically, the connection will be
/// opened until ContentHandler::OnFinish() is called. You can also
/// terminates the connection prematurely by returning a Response
/// during ContentHandler::OnContent().
///
/// \return A unique_ptr to ContentHandler. The ownership of the
/// ContentHandler will be transferred to the Server obviously.
/// The ContentHandler will handle the content in the HTTP request
/// and react accordingly. If the RequestHandler return a null
/// pointer, the server will reply "Internal Server Error" (500)
/// to clients.
///
/// \sa Server, ContentHandler
///
class RequestHandler
{
private:
	using Function = std::function<ContentHandlerPtr(Request&)>;
	
	template <typename Callable>
	class CallableHandler : public ContentHandler
	{
	public:
		CallableHandler(const Callable& h, Request& conn) : m_callable{h}, m_conn{conn} {}
		
		future<Response> OnContent(Request&, const char *, std::size_t) override {return {};}
		future<Response> Finish(Request&) override {return DoFinish();}
		
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
		>::type DoFinish()
		{
			return m_callable(std::move(m_conn));
		}
		
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
			return BrightFuture::make_ready_future(m_callable(std::move(m_conn)));
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
		Function
	>::type Adapt(Callable&& handler)
	{
		return [handler=std::forward<Callable>(handler)](Request& conn)
		{
			return std::make_unique<CallableHandler<Callable>>(handler, conn);
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
		Function
	>::type Adapt(Callable&& handler)
	{
		return std::forward<Callable>(handler);
	}
	
	Function Adapt(Response&& response);
	Function Adapt(const Response& response);
	Function Adapt(http_status status);
	
public:
	template <typename Callable>
	RequestHandler(Callable&& callable) : m_func{Adapt(std::forward<Callable>(callable))}
	{
	}
	
	RequestHandler() = default;
	RequestHandler(RequestHandler&&) = default;
	RequestHandler(const RequestHandler&) = default;
	RequestHandler& operator=(RequestHandler&&) = default;
	RequestHandler& operator=(const RequestHandler&) = default;
	
	ContentHandlerPtr operator()(Request& req) const;

private:
	Function m_func;
};

} // end of namespace
