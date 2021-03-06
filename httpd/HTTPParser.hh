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

#include "Enum.hh"
#include "http-parser/http_parser.h"

#include <iosfwd>

namespace http {

class HTTPParser
{
public:
	enum class Progress {start, header, content, finished};

	class Callback
	{
	public:
		virtual ~Callback() = default;
		
		virtual void OnMessageStart(Method method, Status status, std::string&& url) = 0;
		virtual void OnHeader(std::string&& field, std::string&& value) = 0;
		virtual int OnHeaderComplete() = 0;
		virtual int OnContent(const char *data, std::size_t size) = 0;
		virtual int OnMessageEnd() = 0;
	};

public:
	explicit HTTPParser(Callback& callback);
	HTTPParser(HTTPParser&&) = delete;
	HTTPParser(const HTTPParser&) = delete;
	HTTPParser& operator=(HTTPParser&&) = delete;
	HTTPParser& operator=(const HTTPParser&) = delete;
	
	std::size_t Parse(const char *data, std::size_t size);
	
	http_errno Result() const;
	Progress CurrentProgress() const;
	
	// Request only
	const std::string& URL() const { return m_url;}
	
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
	
	Callback&   m_output;
	Progress    m_progress{Progress::start};
	
	::http_parser_settings  m_setting{};
	::http_parser           m_parser{};
};

std::ostream& operator<<(std::ostream& os, HTTPParser::Progress p);

} // end of namespace
