/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SimpleHttp
    distribution for more details.
*/

//
// Created by nestal on 11/8/17.
//

#include "Adaptors.hh"

namespace http {

RequestHandler MakeHandler(Response&& response)
{
	return [response=std::move(response)](Request&) mutable
	{
		return std::make_unique<IgnoreContent>(std::move(response));
	};
}

RequestHandler MakeHandler(const Response& response)
{
	return [response](Request&) mutable
	{
		return std::make_unique<IgnoreContent>(std::move(response));
	};
}

RequestHandler MakeHandler(http_status status)
{
	return [status](Request&)
	{
		return std::make_unique<IgnoreContent>(Response{status});
	};
}

} // end of namespace
