/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SimpleHttp
    distribution for more details.
*/

//
// Created by nestal on 11/3/17.
//

#include "HTTPParser.hh"

#include <cassert>
#include <ostream>

namespace http {

HTTPParser::HTTPParser(Callback& callback) : m_output{callback}
{
	::http_parser_settings_init(&m_setting);
	m_setting.on_message_begin = [](http_parser*)
	{
		return 0;
	};
	m_setting.on_url = [](http_parser* p, const char *data, size_t size)
	{
		auto pthis = reinterpret_cast<HTTPParser*>(p->data);
		assert(pthis);
		pthis->m_url.append(data, size);
		return 0;
	};
	m_setting.on_header_field = [](http_parser* p, const char *data, size_t size)
	{
		auto pthis = reinterpret_cast<HTTPParser*>(p->data);
		assert(pthis);
		return pthis->OnHeaderField(data, size);
	};
	m_setting.on_header_value = [](http_parser* p, const char *data, size_t size)
	{
		auto pthis = reinterpret_cast<HTTPParser*>(p->data);
		assert(pthis);
		return pthis->OnHeaderValue(data, size);
	};
	m_setting.on_headers_complete = [](http_parser* p)
	{
		auto pthis = reinterpret_cast<HTTPParser*>(p->data);
		assert(pthis);
		pthis->AddHeader();
		pthis->m_progress = Progress::content;
		return pthis->m_output.OnHeaderComplete();
	};
	m_setting.on_body = [](http_parser* p, const char *data, size_t size)
	{
		auto pthis = reinterpret_cast<HTTPParser *>(p->data);
		assert(pthis);
		return pthis->m_output.OnContent(data, size);
	};
	m_setting.on_message_complete = [](http_parser *p)
	{
		auto pthis = reinterpret_cast<HTTPParser *>(p->data);
		pthis->m_progress = Progress::finished;
		return pthis->m_output.OnMessageEnd();
	};
	
	m_parser.data = this;
	::http_parser_init(&m_parser, HTTP_BOTH);
}

std::size_t HTTPParser::Parse(const char *data, std::size_t size)
{
	return ::http_parser_execute(&m_parser, &m_setting, data, size);
}

int HTTPParser::OnHeaderField(const char *data, std::size_t size)
{
	switch (m_header_state)
	{
	case HeaderState::none:
		m_progress = Progress::header;
		m_output.OnMessageStart(Method{m_parser.method}, Status{m_parser.status_code}, std::move(m_url));
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

int HTTPParser::OnHeaderValue(const char *data, std::size_t size)
{
	m_header_value.append(data, size);
	m_header_state = HeaderState::value;
	return 0;
}

void HTTPParser::AddHeader()
{
	m_output.OnHeader(std::move(m_header_field), std::move(m_header_value));
	m_header_field.clear();
	m_header_value.clear();
}

http_errno HTTPParser::Result() const
{
	return static_cast<http_errno>(m_parser.http_errno);
}

HTTPParser::Progress HTTPParser::CurrentProgress() const
{
	return m_progress;
}

std::ostream& operator<<(std::ostream& os, HTTPParser::Progress p)
{
	return os << "HTTPParser::Progress(" << static_cast<int>(p) << ")";
}

} // end of namespace
