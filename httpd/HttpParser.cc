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
	m_setting.on_message_begin = [](http_parser*)
	{
		return 0;
	};
	m_setting.on_url = [](http_parser* p, const char *data, size_t size)
	{
		auto pthis = reinterpret_cast<HttpParser*>(p->data);
		assert(pthis);
		pthis->m_url.append(data, size);
		return 0;
	};
	m_setting.on_header_field = [](http_parser* p, const char *data, size_t size)
	{
		auto pthis = reinterpret_cast<HttpParser*>(p->data);
		assert(pthis);
		return pthis->OnHeaderField(data, size);
	};
	m_setting.on_header_value = [](http_parser* p, const char *data, size_t size)
	{
		auto pthis = reinterpret_cast<HttpParser*>(p->data);
		assert(pthis);
		return pthis->OnHeaderValue(data, size);
	};
	m_setting.on_headers_complete = [](http_parser* p)
	{
		auto pthis = reinterpret_cast<HttpParser*>(p->data);
		assert(pthis);
		pthis->AddHeader();
		return 0;
	};
	m_setting.on_body = [](http_parser* p, const char *data, size_t size)
	{
		auto pthis = reinterpret_cast<HttpParser *>(p->data);
		assert(pthis);
		pthis->m_progress = Progress::content;
		if (pthis->m_output)
			pthis->m_output->OnContent(data, size);
		return 0;
	};
	m_setting.on_message_complete = [](http_parser *p)
	{
		auto pthis = reinterpret_cast<HttpParser *>(p->data);
		pthis->m_progress = Progress::finished;
		return 0;
	};
	
	m_parser.data = this;
	::http_parser_init(&m_parser, HTTP_REQUEST);
}

std::size_t HttpParser::Parse(const char *data, std::size_t size)
{
	return ::http_parser_execute(&m_parser, &m_setting, data, size);
}

int HttpParser::OnHeaderField(const char *data, std::size_t size)
{
	switch (m_header_state)
	{
	case HeaderState::none:
		m_progress = Progress::header;
		if (m_output)
			m_output->OnMessageStart(
				http_method_str(static_cast<http_method >(m_parser.method)),
				std::move(m_url),
				m_parser.http_major,
				m_parser.http_minor
			);
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
	if (m_output)
		m_output->OnHeader(std::move(m_header_field), std::move(m_header_value));
	m_header_field.clear();
	m_header_value.clear();
}

http_errno HttpParser::Result() const
{
	return static_cast<http_errno>(m_parser.http_errno);
}

HttpParser::Progress HttpParser::CurrentProgress() const
{
	return m_progress;
}

void HttpParser::SetCallback(RequestCallback& callback)
{
	m_output = &callback;
}

std::string HttpParser::URL() const
{
	return m_url;
}

http_method HttpParser::Method() const
{
	return static_cast<http_method>(m_parser.method);
}

std::ostream& operator<<(std::ostream& os, HttpParser::Progress p)
{
	return os << "HttpParser::Progress(" << static_cast<int>(p) << ")";
}

} // end of namespace
