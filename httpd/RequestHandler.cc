/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SimpleHttp
    distribution for more details.
*/

//
// Created by nestal on 11/8/17.
//

#include "RequestHandler.hh"

namespace http {

IgnoreContent::IgnoreContent(const Response& response) : m_response{response}
{

}

IgnoreContent::IgnoreContent(Response&& response) : m_response{std::move(response)}
{
}
	
future<Response> IgnoreContent::OnContent(Request&, const char *, std::size_t)
{
	return {};
}

future<Response> IgnoreContent::Finish(Request&)
{
	return BrightFuture::make_ready_future(std::move(m_response));
}

RequestHandler::Function RequestHandler::Adapt(Response&& response)
{
	return [response=std::move(response)](Request&) mutable
	{
		return std::make_unique<IgnoreContent>(std::move(response));
	};
}

RequestHandler::Function RequestHandler::Adapt(const Response& response)
{
	return [response](Request&) mutable
	{
		return std::make_unique<IgnoreContent>(std::move(response));
	};
}

RequestHandler::Function RequestHandler::Adapt(http_status status)
{
	return [status](Request&)
	{
		return std::make_unique<IgnoreContent>(Response{status});
	};
}

ContentHandlerPtr RequestHandler::operator()(Request& req) const
{
	return m_func(req);
}

} // end of namespace
