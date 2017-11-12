/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SimpleHttp
    distribution for more details.
*/

//
// Created by nestal on 11/12/17.
//

#include "ResponseContent.hh"

namespace http {

void BufferedContent::Set(std::vector<char>&& buf)
{
	m_buffer = std::move(buf);
}

void BufferedContent::Send(boost::asio::ip::tcp::socket& socket, const ResponseContent::WriteHandler& callback) const
{
	async_write(socket, boost::asio::buffer(m_buffer), callback);
}

std::size_t BufferedContent::Length() const
{
	return m_buffer.size();
}

std::string BufferedContent::Str() const
{
	return std::string(m_buffer.begin(), m_buffer.end());
}

void StreamContent::Send(boost::asio::ip::tcp::socket& socket, const ResponseContent::WriteHandler& callback) const
{
	async_write(socket, m_buffer.data(), callback);
}

std::size_t StreamContent::Length() const
{
	return m_buffer.size();
}

std::streambuf *StreamContent::rdbuf()
{
	return &m_buffer;
}

std::string StreamContent::Str() const
{
	std::string str(m_buffer.size(), '\0');
	buffer_copy(boost::asio::buffer(&str[0], str.size()), m_buffer.data());
	return str;
}

} // end of namespace
