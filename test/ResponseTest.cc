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
		boost::asio::streambuf sb;
		std::ostream os{&sb};
		os << "Hello";
		
		content_length = sb.size();
		
		subject.SetContent(sb, "text/plain");
	}
	SECTION("no content")
	{
		subject.SetContent(std::vector<char>{}, "text/plain");
	}
	
	ParsedResponse msg;
	HTTPParser parser{msg};
	
	auto str = to_string(subject);
	INFO("Response is " << str);
	auto len = parser.Parse(str.c_str(), str.size());
	CHECK(len == str.size());
	CHECK(parser.Result() == HPE_OK);
	CHECK(msg.status == HTTP_STATUS_OK);
	CHECK(msg.content.size() == content_length);
}
