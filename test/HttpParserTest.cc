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

TEST_CASE("HttpParser simple test", "[normal]")
{
	HttpParser subject;
	const char request[] = "GET /some/path/to/url HTTP/1.1\r\n"
		"Host: localhost:8080\r\n"
		"User-Agent: curl/7.53.1\r\n"
		"Accept: */*\r\n"
		"\r\n";
	
	auto r = subject.Parse(request, sizeof(request)-1);
	CHECK(r == sizeof(request)-1);
	
	auto&& result = subject.Result();
	CHECK(result.Method() == "GET");
	CHECK(result.Headers().Count() == 3);
	CHECK(result.Headers().Field("Host") == "localhost:8080");
}

TEST_CASE("parser test", "[normal]")
{
	std::string url;
	
	http_parser_settings settings{};
	settings.on_url = [](http_parser* p, const char *data, size_t size)
	{
		auto *url = reinterpret_cast<std::string*>(p->data);
		url->append(data, size);
		std::cout << "URL = " << *url << std::endl;
		return 0;
	};
	settings.on_header_field = [](http_parser* p, const char *data, size_t size){return 0;};
	
	http_parser parser;
	http_parser_init(&parser, HTTP_REQUEST);
	parser.data = (void*)&url;
	
	const char request[] = "GET /some/path/to/url HTTP/1.1\r\n"
		"Host: localhost:8080\r\n"
		"User-Agent: curl/7.53.1\r\n"
		"Accept: */*\r\n"
		"\r\n";
	
	const auto size = sizeof(request)-1;
	CHECK(size > 10);
	auto r = http_parser_execute(&parser, &settings, request, 10);
	CHECK(r > 0);
	
	auto r2 = http_parser_execute(&parser, &settings, request+r, size-r);
	CHECK(r2 == size-r);
}
