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
	/// Get a stock reply.
	Response(http_status status = HTTP_STATUS_OK);
	Response(const Response&) = default;
	Response& operator=(const Response&) = default;
	Response(Response&&) = default;
	Response& operator=(Response&&) = default;
	
	Response& SetStatus(http_status status) &;
	Response&& SetStatus(http_status status) &&;
	Response& SetContentType(const std::string& type) &;
	Response&& SetContentType(const std::string& type) &&;
	Response& AddHeader(const std::string& header, const std::string& value) &;
	Response&& AddHeader(const std::string& header, const std::string& value) &&;

	Response& SetContent(std::vector<char>&& buf) &;
	Response& SetContent(const boost::asio::streambuf& buf) &;
	Response&& SetContent(std::vector<char>&& buf) &&;
	Response&& SetContent(const boost::asio::streambuf& buf) &&;
	
	BrightFuture::future<boost::system::error_code> Send(boost::asio::ip::tcp::socket& sock) const;

private:
	/// Convert the reply into a vector of buffers. The buffers do not own the
	/// underlying memory blocks, therefore the reply object must remain valid and
	/// not be changed until the write operation has completed.
	std::vector<boost::asio::const_buffer> ToBuffers() const;
	
private:
	/// Response status, 200=OK
	http_status m_status{HTTP_STATUS_OK};

	/// The header line of "Content-Length:"
	std::string m_content_length;
	
	/// The header line of "Content-Type:"
	std::string m_content_type{"Content-Type: text/html\r\n"};

	std::string m_other_headers;
	
	/// The content to be sent in the reply.
	/// variant<vector, ifstream> ??
	std::vector<char> m_content;
};

} // end of namespace
