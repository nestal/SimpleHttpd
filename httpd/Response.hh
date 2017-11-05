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

#include <boost/asio.hpp>
#include <boost/filesystem/path.hpp>

#include <vector>
#include <ostream>

namespace http {

/// A reply to be sent to a client.
/// The status of the reply.
enum class ResponseStatus
{
	ok = 200,
	created = 201,
	accepted = 202,
	no_content = 204,
	multiple_choices = 300,
	moved_permanently = 301,
	moved_temporarily = 302,
	not_modified = 304,
	bad_request = 400,
	unauthorized = 401,
	forbidden = 403,
	not_found = 404,
	internal_server_error = 500,
	not_implemented = 501,
	bad_gateway = 502,
	service_unavailable = 503
} ;

/**
 * \brief HTTP Response builder
 */
class Response
{
public:
	/// Get a stock reply.
	Response() = default;
	Response(const Response&) = default;
	Response& operator=(const Response&) = default;
	Response(Response&&) = default;
	Response& operator=(Response&&) = default;

	Response(ResponseStatus s);
	
	Response& SetStatus(ResponseStatus s);
	Response& SetContentType(const std::string& type);
	Response& AddHeader(const std::string& header, const std::string& value);

	Response& SetContent(std::vector<char>&& buf);
	Response& SetContent(const boost::asio::streambuf& buf);
	
	/// Convert the reply into a vector of buffers. The buffers do not own the
	/// underlying memory blocks, therefore the reply object must remain valid and
	/// not be changed until the write operation has completed.
	std::vector<boost::asio::const_buffer> ToBuffers() const;

	// void Send(stream_socket& sock);
	// virtual to allow different implementation of content?
	
private:
	ResponseStatus m_status{ResponseStatus::ok};

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
