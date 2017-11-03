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
	int OnHeaderField(const char *data, std::size_t size);
	int OnHeaderValue(const char *data, std::size_t size);
	void AddHeader();
	
private:
	// transient data
	std::string m_url;
	std::string m_header_field;
	std::string m_header_value;
	enum class HeaderState {field, value, none};
	HeaderState m_header_state{HeaderState::none};
	
	Request     m_output;
	
	::http_parser_settings  m_setting{};
	::http_parser           m_parser{};
};

} // end of namespace