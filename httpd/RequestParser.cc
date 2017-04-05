/*
	Copyright © 2016 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SongBits
    distribution for more details.
*/

//
// Created by nestal on 5/12/16.
//

#include "RequestParser.hh"

namespace http {

RequestParser::RequestParser(std::size_t max_length) :
	m_state(method_start),
	m_max_length(max_length)
{
}

void RequestParser::Reset()
{
	m_state = method_start;
	m_header_name.clear();
	m_header_value.clear();
	m_content.clear();
}

RequestParser::Result RequestParser::Consume(Request& req, char input)
{
	switch (m_state)
	{
	case method_start:
		if (!is_char(input) || is_ctl(input) || is_tspecial(input))
		{
			return bad;
		}
		else
		{
			m_state = method;
			m_method.push_back(input);
			return indeterminate;
		}
	case method:
		if (input == ' ')
		{
			req.SetMethod(m_method);
			m_method.clear();

			m_state = uri;
			return indeterminate;
		}
		else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
		{
			return bad;
		}
		else
		{
			m_method.push_back(input);
			return indeterminate;
		}
	case uri:
		if (input == ' ')
		{
			req.SetUri(m_uri);
			m_uri.clear();

			m_state = http_version_h;
			return indeterminate;
		}
		else if (is_ctl(input))
		{
			return bad;
		}
		else
		{
			m_uri.push_back(input);
			return indeterminate;
		}
	case http_version_h:
		if (input == 'H')
		{
			m_state = http_version_t_1;
			return indeterminate;
		}
		else
		{
			return bad;
		}
	case http_version_t_1:
		if (input == 'T')
		{
			m_state = http_version_t_2;
			return indeterminate;
		}
		else
		{
			return bad;
		}
	case http_version_t_2:
		if (input == 'T')
		{
			m_state = http_version_p;
			return indeterminate;
		}
		else
		{
			return bad;
		}
	case http_version_p:
		if (input == 'P')
		{
			m_state = http_version_slash;
			return indeterminate;
		}
		else
		{
			return bad;
		}
	case http_version_slash:
		if (input == '/')
		{
			m_version = 0;
			m_state = http_version_major_start;
			return indeterminate;
		}
		else
		{
			return bad;
		}
	case http_version_major_start:
		if (is_digit(input))
		{
			m_version = m_version * 10 + input - '0';
			m_state = http_version_major;
			return indeterminate;
		}
		else
		{
			return bad;
		}
	case http_version_major:
		if (input == '.')
		{
			req.SetMajorVersion(m_version);
			m_version = 0;

			m_state = http_version_minor_start;
			return indeterminate;
		}
		else if (is_digit(input))
		{
			m_version = m_version * 10 + input - '0';
			return indeterminate;
		}
		else
		{
			return bad;
		}
	case http_version_minor_start:
		if (is_digit(input))
		{
			m_version = m_version * 10 + input - '0';
			m_state = http_version_minor;
			return indeterminate;
		}
		else
		{
			return bad;
		}
	case http_version_minor:
		if (input == '\r')
		{
			req.SetMinorVersion(m_version);
			m_version = 0;

			m_state = expecting_newline_1;
			return indeterminate;
		}
		else if (is_digit(input))
		{
			m_version = m_version * 10 + input - '0';
			return indeterminate;
		}
		else
		{
			return bad;
		}
	case expecting_newline_1:
		if (input == '\n')
		{
			m_state = header_line_start;
			return indeterminate;
		}
		else
		{
			return bad;
		}
	case header_line_start:
		if (input == '\r')
		{
			m_state = expecting_newline_3;
			return indeterminate;
		}
		else if (!req.Headers().Empty() && (input == ' ' || input == '\t'))
		{
			m_state = header_lws;
			return indeterminate;
		}
		else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
		{
			return bad;
		}
		else
		{
			m_header_name.push_back(input);
			m_state = header_name;
			return indeterminate;
		}
	case header_lws:
		if (input == '\r')
		{
			m_state = expecting_newline_2;
			return indeterminate;
		}
		else if (input == ' ' || input == '\t')
		{
			return indeterminate;
		}
		else if (is_ctl(input))
		{
			return bad;
		}
		else
		{
			m_state = header_value;
			m_header_value.push_back(input);
			return indeterminate;
		}
	case header_name:
		if (input == ':')
		{
			m_state = space_before_header_value;
			return indeterminate;
		}
		else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
		{
			return bad;
		}
		else
		{
			m_header_name.push_back(input);
			return indeterminate;
		}
	case space_before_header_value:
		if (input == ' ')
		{
			m_state = header_value;
			return indeterminate;
		}
		else
		{
			return bad;
		}
	case header_value:
		if (input == '\r')
		{
			req.AddHeader({m_header_name, m_header_value});
			m_header_name.clear();
			m_header_value.clear();

			m_state = expecting_newline_2;
			return indeterminate;
		}
		else if (is_ctl(input))
		{
			return bad;
		}
		else
		{
			m_header_value.push_back(input);
			return indeterminate;
		}
	case expecting_newline_2:
		if (input == '\n')
		{
			m_state = header_line_start;
			return indeterminate;
		}
		else
		{
			return bad;
		}
	case expecting_newline_3:
		if (input == '\n')
		{
			m_state = content_body;
			m_content.clear();

			// content length too long, we won't put all these a std::string
			if (req.Headers().ContentLength() >= m_max_length)
				return bad;

			return m_content.size() == req.Headers().ContentLength() ? good : indeterminate;
		}
		else
			return bad;

	case content_body:
		m_content.push_back(input);

		if (m_content.size() == req.Headers().ContentLength())
		{
			req.SetContent(std::move(m_content));
			return good;
		}
		else
			return indeterminate;

	default:
		return bad;
	}
}

bool RequestParser::is_char(int c)
{
	return c >= 0 && c <= 127;
}

bool RequestParser::is_ctl(int c)
{
	return (c >= 0 && c <= 31) || (c == 127);
}

bool RequestParser::is_tspecial(int c)
{
	switch (c)
	{
	case '(': case ')': case '<': case '>': case '@':
	case ',': case ';': case ':': case '\\': case '"':
	case '/': case '[': case ']': case '?': case '=':
	case '{': case '}': case ' ': case '\t':
		return true;
	default:
		return false;
	}
}

bool RequestParser::is_digit(int c)
{
	return c >= '0' && c <= '9';
}

}