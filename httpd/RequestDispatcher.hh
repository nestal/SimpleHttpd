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

#include "RequestHandler.hh"

#include <map>

namespace http {

class RequestDispatcher
{
public:
	RequestDispatcher() = default;

	template <typename Callback>
	void Add(const std::string& uri, Callback&& handler)
	{
		m_map.emplace(uri, std::forward<Callback>(handler));
	}
	void SetDefault(RequestHandler handler);

	BrightFuture::future<Response> HandleRequest(const ConnectionPtr& c) const noexcept ;

private:
	std::map<std::string, RequestHandler> m_map;

	RequestHandler m_default;
};

} // end of namespace
