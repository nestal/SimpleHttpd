/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SimpleHttp
    distribution for more details.
*/

//
// Created by nestal on 11/5/17.
//

#include "Enum.hh"

#include "http-parser/http_parser.h"

#include <unordered_map>

namespace http {

std::string Method::Str() const
{
	return http_method_str(static_cast<http_method>(m_parser_enum));
}

std::ostream& operator<<(std::ostream& os, Method method)
{
	return os << method.Str();
}

std::string Status::Str() const
{
	static const std::unordered_map<unsigned, std::string> status_map =
	{
#define XX(num, name, string) {HTTP_STATUS_##name, "HTTP " #num ": " #string},
		HTTP_STATUS_MAP(XX)
#undef XX
	};
	
	auto i = status_map.find(m_parser_enum);
	return i != status_map.end() ? i->second : "unknown";
}

std::ostream& operator<<(std::ostream& os, Status status)
{
	return os << status.Str();
}

} // end of namespace
