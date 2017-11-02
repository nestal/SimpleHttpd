/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SimpleHttp
    distribution for more details.
*/

//
// Created by nestal on 11/2/17.
//

#include <catch.hpp>

#include "httpd/QueryString.hh"

using namespace http;

TEST_CASE("normal test", "[normal]")
{
	QueryString subject{"field=value"};
	CHECK(subject.Field("field") == "value");
}
