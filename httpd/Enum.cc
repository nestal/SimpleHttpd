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

namespace http {

std::string Method::Str() const
{
	return http_method_str(static_cast<http_method>(m_parser_enum));
}

} // end of namespace
