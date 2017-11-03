/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SimpleHttp
    distribution for more details.
*/

//
// Created by nestal on 11/3/17.
//

#include "HttpParser.hh"

namespace http {

HttpParser::HttpParser()
{
	m_setting.on_message_begin = [](http_parser *p)
	{
		auto pthis = reinterpret_cast<HttpParser*>(p->data);
		pthis->m_output.SetMethod(http_method_str(static_cast<http_method >(p->method)));
//		pthis->m_output.SetMajorVersion(p->http_major);
//		pthis->m_output.SetMinorVersion(p->http_minor);
		return 0;
	};
	m_setting.on_url = [](http_parser* p, const char *data, size_t size)
	{
		auto pthis = reinterpret_cast<HttpParser*>(p->data);
		pthis->m_url.append(data, size);
		return 0;
	};
	
	m_parser.data = this;
	::http_parser_init(&m_parser, HTTP_REQUEST);
}

std::size_t HttpParser::Parse(const char *data, std::size_t size)
{
	return ::http_parser_execute(&m_parser, &m_setting, data, size);
}

const Request& HttpParser::Result() const
{
	return m_output;
}

} // end of namespace
