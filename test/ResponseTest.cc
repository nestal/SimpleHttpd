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

TEST_CASE("Response normal case", "[normal]")
{
	Response subject;
	
	SECTION("add content")
	{
		boost::asio::streambuf sb;
		std::ostream os{&sb};
		os << "Hello";
		subject.SetContent(sb, "text/plain");
	}
	SECTION("no content")
	{
		subject.SetContent(std::vector<char>{}, "text/plain");
	}
	
	auto s = to_string(subject);
	std::cout << s << std::endl;
	
	// use HTTPParser to parse it
	struct Message : HTTPParser::Callback
	{
		Status      status{};
		HeaderList  headers;
		
		void OnMessageStart(Method, Status st, std::string&&) override
		{
			status = st;
		}
		void OnHeader(std::string&& field, std::string&& value) override
		{
			headers.Add({field, value});
		}
		int OnHeaderComplete() override {return 0;}
		int OnContent(const char *, std::size_t ) override {return 0;}
		int OnMessageEnd() override {return 0;}
	} msg;
	HTTPParser parser{msg};
	
	auto len = parser.Parse(s.c_str(), s.size());
	CHECK(len == s.size());
	CHECK(parser.Result() == HPE_OK);
	CHECK(msg.status == HTTP_STATUS_OK);
}