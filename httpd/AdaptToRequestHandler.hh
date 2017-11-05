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
#include <type_traits>

namespace http {

template <typename Callable>
class AdaptCallableToContentHandler : public ContentHandler
{
public:
	AdaptCallableToContentHandler(const Callable& h, Request& conn) : m_callable{h}, m_conn{conn} {}
	
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
>::type AdaptToRequestHandler(Callable&& handler)
{
	return std::forward<Callable>(handler);
}

} // end of namespace
