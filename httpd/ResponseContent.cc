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

void BufferedContent::Send(boost::asio::ip::tcp::socket& socket, const ResponseContent::WriteHandler& callback)
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

void StreamContent::Send(boost::asio::ip::tcp::socket& socket, const ResponseContent::WriteHandler& callback)
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

FileContent::FileContent(boost::filesystem::path path) : m_file{path.string()}
{
}

void FileContent::Send(boost::asio::ip::tcp::socket& socket, const ResponseContent::WriteHandler& callback)
{
	auto count = m_file.rdbuf()->sgetn(&m_buffer[0], m_buffer.size());
	if (count > 0)
		async_write(
			socket,
			boost::asio::buffer(&m_buffer[0], count),
			[&socket, callback, this](const boost::system::error_code& ec, std::size_t)
			{
				if (!ec)
					callback(ec, 0);
				else
					Send(socket, callback);
			}
		);
	else
		callback({}, 0);
}

std::size_t FileContent::Length() const
{
	// not implemented yet
	throw -1;
}

std::string FileContent::Str() const
{
	// not implemented yet
	throw -1;
}

} // end of namespace
