/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SimpleHttp
    distribution for more details.
*/

//
// Created by nestal on 11/12/17.
//

#pragma once

#include <boost/asio.hpp>

namespace http {

class ResponseContent
{
public:
	using WriteHandler = std::function<void(const boost::system::error_code&, std::size_t)>;
	
public:
	virtual ~ResponseContent() = default;
	
	virtual void Send(boost::asio::ip::tcp::socket& socket, const WriteHandler& callback) const = 0;
	virtual std::size_t Length() const = 0;
};

class BufferedContent : public ResponseContent
{
public:
	BufferedContent() = default;
	
	void Set(std::vector<char>&& buf);
	
	void Send(boost::asio::ip::tcp::socket& socket, const WriteHandler& callback) const override ;
	std::size_t Length() const override;
	
private:
	std::vector<char>   m_buffer;
};

class StreamContent : public ResponseContent
{
public:
	StreamContent() = default;
	
	template <typename T>
	friend StreamContent& operator<<(StreamContent& self, T&& t)
	{
		self.m_str << std::forward<T>(t);
		return self;
	}
	
	void Send(boost::asio::ip::tcp::socket& socket, const WriteHandler& callback) const override ;
	std::size_t Length() const override;
	
private:
	boost::asio::streambuf  m_buf;
	std::ostream            m_str{&m_buf};
};

} // end of namespace
