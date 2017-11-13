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
#include "ResponseContent.hh"

#include "Enum.hh"

#include <boost/asio.hpp>
#include <boost/filesystem/path.hpp>

#include <vector>
#include <iosfwd>

namespace http {

/**
 * \brief HTTP Response builder
 */
class Response
{
public:
	Response(Status status = status_OK);
	Response(const Response&) = default;
	Response& operator=(const Response&) = default;
	Response(Response&&) = default;
	Response& operator=(Response&&) = default;
	
	Response& SetStatus(Status status) &;
	Response&& SetStatus(Status status) &&;
	Response& AddHeader(const std::string& header, const std::string& value) &;
	Response&& AddHeader(const std::string& header, const std::string& value) &&;

	Response& SetContent(std::shared_ptr<ResponseContent> content, const std::string& content_type) &;
	Response&& SetContent(std::shared_ptr<ResponseContent> content, const std::string& content_type) &&;
	
	template <typename AsyncWriteStream, typename Callback>
	void Send(AsyncWriteStream& sock, Callback&& callback) const
	{
		async_write(
			sock,
			HeaderAsBuffers(),
			[callback=std::forward<Callback>(callback), this, &sock](const boost::system::error_code& ec, std::size_t count)
			{
				if (!ec && m_content)
					SendContent(sock, 0, std::forward<decltype(callback)>(callback), count);
				else
					callback(ec, count);
			});
	}

private:
	template <typename AsyncWriteStream, typename Callback>
	void SendContent(AsyncWriteStream& sock, std::size_t pos, Callback&& callback, std::size_t header_size) const
	{
		if (pos < m_content->Length())
			async_write(
				sock,
				buffer(m_content->Get(pos)),
				[&sock, callback=std::forward<decltype(callback)>(callback), this, pos, header_size](const boost::system::error_code& ec, std::size_t count)
				{
					auto next = pos + count;
					if (!ec)
						callback(ec, header_size+next);
					else
						SendContent(sock, next, callback, header_size);
				}
			);
		else
			callback(boost::system::error_code{}, header_size);
	}
	
private:
	/// Convert the reply into a vector of buffers. The buffers do not own the
	/// underlying memory blocks, therefore the reply object must remain valid and
	/// not be changed until the write operation has completed.
	std::vector<boost::asio::const_buffer> HeaderAsBuffers() const;

	friend std::ostream& operator<<(std::ostream& os, const Response& e);
	
private:
	/// Response status, 200=OK
	Status m_status{status_OK};

	/// The header line of "Content-Type:"
	std::string m_content_header{"Content-Type: text/html\r\nContent-Length: 0\r\n"};

	std::string m_other_headers;
	
	std::shared_ptr<ResponseContent> m_content;
};

} // end of namespace
