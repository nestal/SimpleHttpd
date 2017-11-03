/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SimpleHttp
    distribution for more details.
*/

//
// Created by nestal on 11/3/17.
//

#include <iostream>
#include "http-parser/http_parser.h"
#include "httpd/HttpParser.hh"

#include "BrightFuture/test/catch.hpp"

using namespace http;

TEST_CASE("HttpParser GET simple request", "[normal]")
{
	HttpParser subject;
	const char request[] = "GET /some/path/to/url HTTP/1.1\r\n"
		"Host: localhost:8080\r\n"
		"User-Agent: curl/7.53.1\r\n"
		"Accept: */*\r\n"
		"\r\n";
	const auto size = sizeof(request)-1;
	
	SECTION("one pass")
	{
		auto r = subject.Parse(request, size);
		CHECK(r == size);
		CHECK(subject.Errno() == HPE_OK);
		
		auto&& result = subject.Result();
		CHECK(result.Uri() == "/some/path/to/url");
		CHECK(result.Method() == "GET");
		CHECK(result.Headers().Count() == 3);
		CHECK(result.Headers().Field("Host") == "localhost:8080");
	}
	SECTION("two pass")
	{
		const auto pass1 = 10;
		CHECK(size > pass1);
		auto r = subject.Parse(request, pass1);
		CHECK(r > 0);
		CHECK(r <= pass1);
		
		auto r2 = subject.Parse(request+r, size-r);
		CHECK(r2 == size-r);
		CHECK(subject.Errno() == HPE_OK);
		
		auto&& result = subject.Result();
		CHECK(result.Method() == "GET");
		CHECK(result.Headers().Count() == 3);
	}
}

TEST_CASE("HttpParser POST request content", "[normal]")
{
	const char request[] =
		"POST / HTTP/1.1\r\n"
		"Host: localhost:8080\r\n"
		"User-Agent: curl/7.53.1\r\n"
		"Accept: */*\r\n"
		"Content-Length: 12\r\n"
		"Content-Type: application/x-www-form-urlencoded\r\n"
		"\r\n"
		"hello=world!\r\n"
	;
	const auto size = sizeof(request)-1;
	
	HttpParser subject;
	auto r = subject.Parse(request, size);
	CHECK(r == size);
	CHECK(subject.Errno() == HPE_OK);
	
	auto&& result = subject.Result();
	CHECK(result.Method() == "POST");
	CHECK(result.Content() == "hello=world!");
}
