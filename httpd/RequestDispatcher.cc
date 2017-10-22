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

#include "Request.hh"
#include "Response.hh"
#include "UriString.hh"

#include <cassert>

namespace http {

void RequestDispatcher::HandleRequest(const ConnectionPtr& c) const noexcept
{
	assert(c);
	
	try
	{
		UriString uri{c->Request().Uri()};
		
		auto it = m_map.find(uri[0]);
		auto& handler = (it == m_map.end() ? m_default : it->second);
		handler(c);
	}
	catch (std::exception& e)
	{
		c->Response().SetStatus(ResponseStatus::internal_server_error);
		
		std::ostream os{&c->Response().Content()};
		os << e.what();
	}
	catch (...)
	{
		c->Response().SetStatus(ResponseStatus::internal_server_error);
	}
}

void RequestDispatcher::SetDefault(RequestHandler handler)
{
	m_default = handler;
}

} // end of namespace
