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
#include <boost/filesystem.hpp>

#include <string>

namespace http {

class ResponseContent
{
public:
	using WriteHandler = std::function<void(const boost::system::error_code&, std::size_t)>;
	
public:
	virtual ~ResponseContent() = default;
	
	virtual void Send(boost::asio::ip::tcp::socket& socket, const WriteHandler& callback) = 0;
	virtual std::size_t Length() const = 0;
	virtual std::string Str() const = 0;
};

class BufferedContent : public ResponseContent
{
public:
	BufferedContent() = default;
	
	void Set(std::vector<char>&& buf);
	
	void Send(boost::asio::ip::tcp::socket& socket, const WriteHandler& callback) override ;
	std::size_t Length() const override;
	std::string Str() const override;
	
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
	
	std::streambuf* rdbuf();
	
	void Send(boost::asio::ip::tcp::socket& socket, const WriteHandler& callback) override ;
	std::size_t Length() const override;
	std::string Str() const override;
	
private:
	boost::asio::streambuf  m_buffer;
	std::ostream            m_str{&m_buffer};
};

class FileContent : public ResponseContent
{
public:
	explicit FileContent(boost::filesystem::path path);

	void Send(boost::asio::ip::tcp::socket& socket, const WriteHandler& callback) override ;
	std::size_t Length() const override;
	std::string Str() const override;
	
private:
	std::array<char, 1024>  m_buffer;
	std::ifstream           m_file;
};

} // end of namespace
