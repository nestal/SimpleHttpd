/*
	Copyright © 2016 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SongBits
    distribution for more details.
*/

//
// Created by nestal on 5/14/16.
//

#pragma once

#include "BrightFuture.hh"

#include "http-parser/http_parser.h"

#include <boost/asio.hpp>
#include <boost/filesystem/path.hpp>

#include <vector>
#include <ostream>

namespace http {

/**
 * \brief HTTP Response builder
 */
class Response
{
public:
	Response(http_status status = HTTP_STATUS_OK);
	Response(const Response&) = default;
	Response& operator=(const Response&) = default;
	Response(Response&&) = default;
	Response& operator=(Response&&) = default;
	
	Response& SetStatus(http_status status) &;
	Response&& SetStatus(http_status status) &&;
	Response& AddHeader(const std::string& header, const std::string& value) &;
	Response&& AddHeader(const std::string& header, const std::string& value) &&;

	Response& SetContent(std::vector<char>&& buf, const std::string& content_type) &;
	Response& SetContent(const boost::asio::streambuf& buf, const std::string& content_type) &;
	Response&& SetContent(std::vector<char>&& buf, const std::string& content_type) &&;
	Response&& SetContent(const boost::asio::streambuf& buf, const std::string& content_type) &&;
	
	template <typename AsyncWriteStream>
	auto Send(AsyncWriteStream& sock) const
	{
		auto promise = std::make_shared<BrightFuture::promise<boost::system::error_code>>();
		async_write(sock, ToBuffers(), [promise](boost::system::error_code ec, std::size_t count)
		{
			promise->set_value(ec);
		});
		return promise->get_future();
	}

private:
	/// Convert the reply into a vector of buffers. The buffers do not own the
	/// underlying memory blocks, therefore the reply object must remain valid and
	/// not be changed until the write operation has completed.
	std::vector<boost::asio::const_buffer> ToBuffers() const;

	friend std::ostream& operator<<(std::ostream& os, const Response& e);
	friend std::string to_string(const Response& e);
	
private:
	/// Response status, 200=OK
	http_status m_status{HTTP_STATUS_OK};

	/// The header line of "Content-Type:"
	std::string m_content_header{"Content-Type: text/html\r\nContent-Length: 0\r\n"};

	std::string m_other_headers;
	
	/// The content to be sent in the reply.
	/// variant<vector, ifstream> ??
	std::vector<char> m_content;
};

} // end of namespace
