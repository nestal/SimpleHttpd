/*
	Copyright © 2016 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SongBits
    distribution for more details.
*/

//
// Created by nestal on 5/14/16.
//

#include "Response.hh"

#include <unordered_map>

namespace http {

using namespace boost::asio;

namespace misc_strings {

const char name_value_separator[] = { ':', ' ' };
const char crlf[] = { '\r', '\n' };

const std::unordered_map<http_status, std::string> status_map =
{
#define XX(num, name, string) {HTTP_STATUS_##name, "HTTP/1.0 " #num " " #string "\r\n"},
  HTTP_STATUS_MAP(XX)
#undef XX
};

} // namespace misc_strings

std::vector<const_buffer> Response::ToBuffers() const
{
	auto status = misc_strings::status_map.find(m_status);
	if (status == misc_strings::status_map.end())
		status = misc_strings::status_map.find(HTTP_STATUS_BAD_REQUEST);
	assert(status != misc_strings::status_map.end());
	
	return std::vector<const_buffer>{
		buffer(status->second),
		buffer(m_content_type),
		buffer(m_content_length),
		buffer(m_other_headers),
		buffer(misc_strings::crlf),
		buffer(m_content)
	};
}

Response& Response::SetStatus(http_status status)
{
	// e.g. HTTP/1.0 200 OK
//	m_status = "HTTP/1.0 " + std::to_string(static_cast<int>(status)) + " " + misc_strings::status_map[status] + "\r\n";
	m_status = status;
	return *this;
}

Response& Response::SetContentType(const std::string& type)
{
	m_content_type = "Content-Type: " + type + "\r\n";
	return *this;
}

Response& Response::AddHeader(const std::string& header, const std::string& value)
{
	m_other_headers += (header + ": " + value + "\r\n");
	return *this;
}

Response& Response::SetContent(std::vector<char>&& buf)
{
	m_content = std::move(buf);
	
	// construct Content-Length header with the actual length of content
	m_content_length = "Content-Length: " + std::to_string(m_content.size()) + "\r\n";
	
	return *this;
}

Response& Response::SetContent(const boost::asio::streambuf& buf)
{
	std::vector<char> content(buf.size());
	buffer_copy(boost::asio::buffer(content), buf.data());
	
	return SetContent(std::move(content));
}

Response::Response(http_status s) : m_status{s}
{
}

} // end of namespace
