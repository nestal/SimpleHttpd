/*
	Copyright © 2016 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SongBits
    distribution for more details.
*/

//
// Created by nestal on 5/12/16.
//

#pragma once

#include <tuple>
#include "Request.hh"
namespace http {

class RequestParser
{
public:
	/// Construct ready to parse the request method.
	explicit RequestParser(std::size_t max_length = 10 * 1024 * 1024);

	/// Reset to initial parser state.
	void Reset();

	/// Result of parse.
	enum Result
	{
		good, bad, indeterminate
	};

	/// Parse some data. The enum return value is good when a complete request has
	/// been parsed, bad if the data is invalid, indeterminate when more data is
	/// required. The InputIterator return value indicates how much of the input
	/// has been consumed.
	template<typename InputIterator>
	std::tuple <Result, InputIterator> Parse(
		Request& req,
		InputIterator begin, InputIterator end
	)
	{
		while (begin != end)
		{
			Result result = Consume(req, *begin++);
			if (result == good || result == bad)
				return std::make_tuple(result, begin);
		}
		return std::make_tuple(indeterminate, begin);
	}

private:
	/// Handle the next character of input.
	Result Consume(Request& req, char input);

	/// Check if a byte is an HTTP character.
	static bool is_char(int c);

	/// Check if a byte is an HTTP control character.
	static bool is_ctl(int c);

	/// Check if a byte is defined as an HTTP tspecial character.
	static bool is_tspecial(int c);

	/// Check if a byte is a digit.
	static bool is_digit(int c);

	/// The current state of the parser.
	enum State
	{
		method_start,
		method,
		uri,
		http_version_h,
		http_version_t_1,
		http_version_t_2,
		http_version_p,
		http_version_slash,
		http_version_major_start,
		http_version_major,
		http_version_minor_start,
		http_version_minor,
		expecting_newline_1,
		header_line_start,
		header_lws,
		header_name,
		space_before_header_value,
		header_value,
		expecting_newline_2,
		expecting_newline_3,
		content_body
	} m_state;

	std::string m_method;
	int m_version;
	std::string m_uri;

	std::string m_header_name;
	std::string m_header_value;
	std::size_t m_max_length;

	std::string m_content;
};

} // end of namespace
