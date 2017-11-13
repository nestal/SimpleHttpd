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

std::size_t BufferedContent::Length() const
{
	return m_buffer.size();
}

boost::asio::const_buffer BufferedContent::Get(std::size_t start) const
{
	return {&m_buffer[start], m_buffer.size() - start};
}

std::size_t StreamContent::Length() const
{
	return m_buffer.size();
}

std::streambuf* StreamContent::rdbuf()
{
	return &m_buffer;
}

boost::asio::const_buffer StreamContent::Get(std::size_t start) const
{
	return start == 0 ? m_buffer.data() : boost::asio::const_buffer{};
}

FileContent::FileContent(boost::filesystem::path path) : m_file{path.string()}
{
}

std::size_t FileContent::Length() const
{
	// not implemented yet
	throw -1;
}

boost::asio::const_buffer FileContent::Get(std::size_t start) const
{
	if (start != static_cast<std::size_t>(m_file.tellg()))
		m_file.seekg(start);
		
	auto count = m_file.rdbuf()->sgetn(&m_buffer[0], m_buffer.size());
	
	return {&m_buffer[0], static_cast<std::size_t>(count)};
}

} // end of namespace
