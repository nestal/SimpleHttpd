/*
	Copyright © 2016 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SongBits
    distribution for more details.
*/

//
// Created by nestal on 5/15/16.
//

#include "RequestDispatcher.hh"

#include "Response.hh"
#include "UriString.hh"

namespace http {

ContentHandlerPtr RequestDispatcher::HandleRequest(Request& req) const noexcept
{
	UriString uri{req.URL()};
	
	auto it = m_map.find(uri[0]);
	auto& handler = (it == m_map.end() ? m_default : it->second);
	
	try
	{
		return handler(req);
	}
	catch (...)
	{
		assert(m_exception_handler);
		return m_exception_handler(std::current_exception());
	}
}

void RequestDispatcher::Add(const std::string& uri, RequestHandler handler)
{
	m_map.emplace(uri, std::move(handler));
}

void RequestDispatcher::SetDefault(RequestHandler handler)
{
	m_default = std::move(handler);
}

void RequestDispatcher::SetExceptionHandler(ExceptionHandler handler)
{
	assert(handler);
	m_exception_handler = std::move(handler);
}

} // end of namespace
