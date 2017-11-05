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

ContentHandlerPtr RequestDispatcher::HandleRequest(const ConnectionPtr& c) const noexcept
{
	assert(c);
	
	UriString uri{c->Request().Uri()};
	
	auto it = m_map.find(uri[0]);
	auto& handler = (it == m_map.end() ? m_default : it->second);
	return handler(c);
}

void RequestDispatcher::SetDefault(RequestHandler handler)
{
	m_default = std::move(handler);
}

} // end of namespace
