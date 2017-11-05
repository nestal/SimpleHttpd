/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SimpleHttp
    distribution for more details.
*/

//
// Created by nestal on 11/5/17.
//

#pragma once

#include <string>
#include <iosfwd>

namespace http {

class Method
{
public:
	Method(int parser_enum = 1) : m_parser_enum{parser_enum} {}
	
	friend bool operator==(const Method& lhs, const Method& rhs) {return lhs.m_parser_enum == rhs.m_parser_enum;}
	friend bool operator!=(const Method& lhs, const Method& rhs) {return lhs.m_parser_enum != rhs.m_parser_enum;}
	friend bool operator> (const Method& lhs, const Method& rhs) {return lhs.m_parser_enum >  rhs.m_parser_enum;}
	friend bool operator>=(const Method& lhs, const Method& rhs) {return lhs.m_parser_enum >= rhs.m_parser_enum;}
	friend bool operator< (const Method& lhs, const Method& rhs) {return lhs.m_parser_enum <  rhs.m_parser_enum;}
	friend bool operator<=(const Method& lhs, const Method& rhs) {return lhs.m_parser_enum <= rhs.m_parser_enum;}
	
	std::string Str() const;
	int Get() const {return m_parser_enum;}
	
private:
	int m_parser_enum;
};

std::ostream& operator<<(std::ostream& os, Method method);

} // end of namespace
