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
#include <iostream>

namespace {

const char name_value_separator[] = {':', ' '};
const char crlf[] = {'\r', '\n'};

const std::string& StatusString(http_status status)
{
	static const std::unordered_map<http_status, std::string> status_map =
	{
#define XX(num, name, string) {HTTP_STATUS_##name, "HTTP/1.0 " #num " " #string "\r\n"},
		HTTP_STATUS_MAP(XX)
#undef XX
	};
	
	auto it = status_map.find(status);
	if (it == status_map.end())
		it = status_map.find(HTTP_STATUS_BAD_REQUEST);
	assert(it != status_map.end());
	return it->second;
}

} // end of anonymouse namespace

namespace http {

using namespace boost::asio;

Response::Response(http_status s) : m_status{s}
{
}

std::vector<const_buffer> Response::ToBuffers() const
{
	static const std::string content_length = "Content-Length: ";
	
	return {
		buffer(StatusString(m_status)),
		buffer(m_content_type),
		buffer(content_length), buffer(m_content_length), buffer(crlf),
		buffer(m_other_headers),
		buffer(crlf),
		buffer(m_content)
	};
}

Response& Response::SetStatus(http_status status) &
{
	m_status = status;
	return *this;
}

Response&& Response::SetStatus(http_status status)&&
{
	m_status = status;
	return std::move(*this);
}

Response& Response::SetContentType(const std::string& type) &
{
	m_content_type = "Content-Type: " + type + "\r\n";
	return *this;
}

Response&& Response::SetContentType(const std::string& type) &&
{
	m_content_type = "Content-Type: " + type + "\r\n";
	return std::move(*this);
}

Response& Response::AddHeader(const std::string& header, const std::string& value) &
{
	m_other_headers += (header + ": " + value + "\r\n");
	return *this;
}

Response&& Response::AddHeader(const std::string& header, const std::string& value) &&
{
	m_other_headers += (header + ": " + value + "\r\n");
	return std::move(*this);
}

Response& Response::SetContent(std::vector<char>&& buf, const std::string& content_type) &
{
	m_content = std::move(buf);
	
	// construct Content-Length header with the actual length of content
	m_content_length = std::to_string(m_content.size());
	SetContentType(content_type);
	
	return *this;
}

Response&& Response::SetContent(std::vector<char>&& buf, const std::string& content_type) &&
{
	m_content = std::move(buf);
	
	// construct Content-Length header with the actual length of content
	m_content_length = std::to_string(m_content.size());
	SetContentType(content_type);
	
	return std::move(*this);
}

Response& Response::SetContent(const boost::asio::streambuf& buf, const std::string& content_type) &
{
	std::vector<char> content(buf.size());
	buffer_copy(boost::asio::buffer(content), buf.data());
	
	return SetContent(std::move(content), content_type);
}

Response&& Response::SetContent(const boost::asio::streambuf& buf, const std::string& content_type) &&
{
	std::vector<char> content(buf.size());
	buffer_copy(boost::asio::buffer(content), buf.data());
	
	return std::move(SetContent(std::move(content), content_type));
}

BrightFuture::future<boost::system::error_code> Response::Send(ip::tcp::socket& sock) const
{
	auto promise = std::make_shared<BrightFuture::promise<boost::system::error_code>>();
	async_write(sock, ToBuffers(), [promise](boost::system::error_code ec, std::size_t count)
	{
		std::cout << "on sent(): " << ec << " " << count << " bytes" << std::endl;
		promise->set_value(ec);
	});
	return promise->get_future();
}

std::ostream& operator<<(std::ostream& os, const Response& e)
{
	for (auto&& buf : e.ToBuffers())
		os.rdbuf()->sputn(buffer_cast<const char*>(buf), buffer_size(buf));
	return os;
}

std::string to_string(const Response& e)
{
	auto buf = e.ToBuffers();
	auto len = buffer_size(buf);
	
	std::string result(len, '\0');
	buffer_copy(buffer(&result[0], len), buf);
	return result;
}

} // end of namespace
