/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SimpleHttp
    distribution for more details.
*/

//
// Created by nestal on 11/3/17.
//

#include "http-parser/http_parser.h"
#include "httpd/HTTPParser.hh"
#include "httpd/RequestHandler.hh"
#include "httpd/HeaderList.hh"

#include "BrightFuture/test/catch.hpp"

#include <iostream>

using namespace http;

struct MockRequest : RequestCallback
{
	Method method;
	std::string url;
	HeaderList header;
	std::string content;
	
	void OnMessageStart(http::Method met, std::string&& u, int, int) override
	{
		method = met;
		url = std::move(u);
	}
	void OnHeader(std::string&& field, std::string&& value) override
	{
		header.Add({std::move(field), std::move(value)});
	}
	int OnHeaderComplete() override
	{
		return 0;
	}
	int OnContent(const char *data, std::size_t size) override
	{
		content.append(data, size);
		return 0;
	}
	int OnMessageEnd() override
	{
		return 0;
	}
};

TEST_CASE("HTTPParser GET simple request", "[normal]")
{
	MockRequest result;
	HTTPParser subject{result};
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
		CHECK(subject.CurrentProgress() == HTTPParser::Progress::finished);
		CHECK(subject.Result() == HPE_OK);
		
		CHECK(result.url == "/some/path/to/url");
		CHECK(result.method == HTTP_GET);
		CHECK(result.header.Count() == 3);
		CHECK(result.header.Field("Host") == "localhost:8080");
	}
	SECTION("two pass")
	{
		const auto pass1 = 40;
		CHECK(size > pass1);
		auto r = subject.Parse(request, pass1);
		CHECK(r > 0);
		CHECK(r <= pass1);
		
		auto r2 = subject.Parse(request+r, size-r);
		CHECK(r2 == size-r);
		CHECK(subject.Result() == HPE_OK);
		
		CHECK(result.method == HTTP_GET);
		CHECK(result.header.Count() == 3);
	}
}

TEST_CASE("HTTPParser POST request content", "[normal]")
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

	MockRequest result;
	HTTPParser subject{result};
	
	auto r = subject.Parse(request, size);
	CHECK(r == size);
	CHECK(subject.CurrentProgress() == HTTPParser::Progress::finished);
	CHECK(subject.Result() == HPE_OK);
	
	CHECK(result.method == HTTP_POST);
	CHECK(result.content == "hello=world!");
}
