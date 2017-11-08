/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SimpleHttp
    distribution for more details.
*/

//
// Created by nestal on 11/6/17.
//

#pragma once

#include "RequestHandler.hh"
#include "Response.hh"

#include <type_traits>

namespace http {

template <typename Callable>
class AdaptCallableToContentHandler : public ContentHandler
{
public:
	AdaptCallableToContentHandler(const Callable& h, Request& conn) : m_callable{h}, m_conn{conn} {}
	
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
	RequestHandler
>::type MakeHandler(Callable&& handler)
{
	return [handler=std::forward<Callable>(handler)](Request& conn)
	{
		return std::make_unique<AdaptCallableToContentHandler<Callable>>(handler, conn);
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
>::type MakeHandler(Callable&& handler)
{
	return std::forward<Callable>(handler);
}

class IgnoreContent : public ContentHandler
{
public:
	explicit IgnoreContent(const Response& response) : m_response{response} {}
	explicit IgnoreContent(Response&& response) : m_response{std::move(response)} {}
	
	future<Response> OnContent(Request&, const char *, std::size_t) override {return {};}
	future<Response> Finish(Request&) override {return BrightFuture::make_ready_future(std::move(m_response));}

private:
	Response m_response;
};

// This overload of Adapt() will only be enabled if the parameter is a Response itself.
RequestHandler MakeHandler(Response&& response);
RequestHandler MakeHandler(const Response& response);
RequestHandler MakeHandler(http_status status);

} // end of namespace