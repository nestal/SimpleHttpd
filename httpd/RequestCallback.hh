/*
	Copyright © 2016 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SongBits
    distribution for more details.
*/

//
// Created by nestal on 5/11/16.
//

#pragma once

#include "Enum.hh"

#include <string>

namespace http {

class RequestCallback
{
public:
	virtual ~RequestCallback() = default;
	
	virtual void OnMessageStart(http::Method method, std::string&& url, int major, int minor) = 0;
	virtual void OnHeader(std::string&& field, std::string&& value) = 0;
	virtual int OnHeaderComplete() = 0;
	virtual int OnContent(const char *data, std::size_t size) = 0;
	virtual int OnMessageEnd() = 0;
};

} // end of namespace
