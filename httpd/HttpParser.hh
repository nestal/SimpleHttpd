/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SimpleHttp
    distribution for more details.
*/

//
// Created by nestal on 11/3/17.
//

#pragma once

#include "Request.hh"
#include "http-parser/http_parser.h"

namespace http {

class HttpParser
{
public:
	HttpParser();
	
	std::size_t Parse(const char *data, std::size_t size);
	
	const Request& Result() const;
	
private:
	// transient data
	std::string m_url;
	
	Request     m_output;
	
	::http_parser_settings  m_setting{};
	::http_parser           m_parser{};
};

} // end of namespace
