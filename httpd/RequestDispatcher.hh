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

#include <unordered_map>

namespace http {

class RequestDispatcher
{
public:
	RequestDispatcher() = default;

	void Add(const std::string& uri, RequestHandler handler);
	void SetDefault(RequestHandler handler);

	ContentHandlerPtr HandleRequest(const ConnectionPtr& c) const noexcept ;

private:
	std::unordered_map<std::string, RequestHandler> m_map;

	RequestHandler m_default;
};

} // end of namespace
