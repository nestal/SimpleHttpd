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
		return 0;
	};
	m_setting.on_url = [](http_parser* p, const char *data, size_t size)
	{
		auto pthis = reinterpret_cast<HttpParser*>(p->data);
		pthis->m_url.append(data, size);
		return 0;
	};
	m_setting.on_header_field = [](http_parser* p, const char *data, size_t size)
	{
		auto pthis = reinterpret_cast<HttpParser*>(p->data);
		return pthis->OnHeaderField(data, size);
	};
	m_setting.on_header_value = [](http_parser* p, const char *data, size_t size)
	{
		auto pthis = reinterpret_cast<HttpParser*>(p->data);
		return pthis->OnHeaderValue(data, size);
	};
	m_setting.on_headers_complete = [](http_parser* p)
	{
		auto pthis = reinterpret_cast<HttpParser*>(p->data);
		pthis->AddHeader();
		return 0;
	};
	m_setting.on_body = [](http_parser* p, const char *data, size_t size)
	{
		auto pthis = reinterpret_cast<HttpParser *>(p->data);
		pthis->m_content.append(data, size);
		return 0;
	};
	m_setting.on_message_complete = [](http_parser *p)
	{
		auto pthis = reinterpret_cast<HttpParser *>(p->data);
		pthis->m_output.SetContent(std::move(pthis->m_content));
		pthis->m_complete = true;
		return 0;
	};
	
	m_parser.data = this;
	::http_parser_init(&m_parser, HTTP_REQUEST);
}

std::size_t HttpParser::Parse(const char *data, std::size_t size)
{
	return ::http_parser_execute(&m_parser, &m_setting, data, size);
}

Request&& HttpParser::Result()
{
	return std::move(m_output);
}

const Request& HttpParser::Result() const
{
	return m_output;
}

int HttpParser::OnHeaderField(const char *data, std::size_t size)
{
	switch (m_header_state)
	{
	case HeaderState::none:
		m_output.SetMajorVersion(m_parser.http_major);
		m_output.SetMinorVersion(m_parser.http_minor);
		m_output.SetUri(m_url);
		break;
	
	case HeaderState::value:
		AddHeader();
		break;
		
	default:
		break;
	}
	m_header_field.append(data, size);
	m_header_state = HeaderState::field;
	return 0;
}

int HttpParser::OnHeaderValue(const char *data, std::size_t size)
{
	m_header_value.append(data, size);
	m_header_state = HeaderState::value;
	return 0;
}

void HttpParser::AddHeader()
{
	m_output.AddHeader({m_header_field, m_header_value});
	m_header_field.clear();
	m_header_value.clear();
}

http_errno HttpParser::Errno() const
{
	return static_cast<http_errno>(m_parser.http_errno);
}

bool HttpParser::Complete() const
{
	return m_complete;
}

} // end of namespace
