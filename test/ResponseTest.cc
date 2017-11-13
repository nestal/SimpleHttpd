/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SimpleHttp
    distribution for more details.
*/

//
// Created by nestal on 11/9/17.
//

#include <catch.hpp>

#include "httpd/Response.hh"
#include "httpd/HTTPParser.hh"
#include "httpd/HeaderList.hh"

#include <iostream>
using namespace http;

struct AsyncStringStream
{
	boost::asio::io_service&    ios;
	std::string                 str;
	
	auto& get_io_service()
	{
		return ios;
	}
	
	template <typename ConstBuffers, typename Callback>
	void async_write_some(ConstBuffers&& bufs, Callback&& callback)
	{
		std::size_t old = str.size();
		for (auto&& buf: bufs)
			str.append(boost::asio::buffer_cast<const char*>(buf), boost::asio::buffer_size(buf));
		
		std::forward<Callback>(callback)(boost::system::error_code{}, str.size() - old);
	}
};

// use HTTPParser to parse it
struct ParsedResponse : HTTPParser::Callback
{
	Status      status{};
	HeaderList  headers;
	std::string content;
	
	void OnMessageStart(Method, Status s, std::string&&) override
	{
		status = s;
	}
	void OnHeader(std::string&& field, std::string&& value) override
	{
		headers.Add({field, value});
	}
	int OnHeaderComplete() override {return 0;}
	int OnContent(const char *data, std::size_t size) override
	{
		content.append(data, size);
		return 0;
	}
	int OnMessageEnd() override {return 0;}
};

TEST_CASE("Response normal case", "[normal]")
{
	Response subject;
	std::size_t content_length = 0;
	
	SECTION("add content")
	{
		auto sb = std::make_shared<StreamContent>();
		*sb << "Hello";
		
		content_length = sb->Length();
		
		subject.SetContent(sb, "text/plain");
	}
	SECTION("no content")
	{
		subject.SetContent(std::make_shared<BufferedContent>(), "text/plain");
	}
	
	ParsedResponse msg;
	HTTPParser parser{msg};

	boost::asio::io_service ios;
	AsyncStringStream ss{ios};
	
	bool run = false;
	std::size_t total = 0;
	subject.Send(ss, [&run, &total](auto&&, auto count){run = true; total = count;});
	ios.run();
	CHECK(run);
	CHECK(total == ss.str.size());
	
	INFO("Response is " << ss.str);
	auto len = parser.Parse(ss.str.c_str(), ss.str.size());
	CHECK(len == ss.str.size());
	CHECK(parser.Result() == HPE_OK);
	CHECK(msg.status == status_OK);
	CHECK(msg.content.size() == content_length);
}
