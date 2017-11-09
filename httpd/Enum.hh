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

#include "http-parser/http_parser.h"

#include <string>
#include <iosfwd>

namespace http {

class Method
{
public:
	using Enum = ::http_method;
	
public:
	constexpr explicit Method(Enum parser_enum = static_cast<Enum>(HTTP_GET)) : m_parser_enum{parser_enum} {}
	constexpr explicit Method(unsigned parser_enum) : m_parser_enum{static_cast<Enum>(parser_enum)} {}
	
	friend bool operator==(const Method& lhs, const Method& rhs) {return lhs.m_parser_enum == rhs.m_parser_enum;}
	friend bool operator!=(const Method& lhs, const Method& rhs) {return lhs.m_parser_enum != rhs.m_parser_enum;}
	friend bool operator> (const Method& lhs, const Method& rhs) {return lhs.m_parser_enum >  rhs.m_parser_enum;}
	friend bool operator>=(const Method& lhs, const Method& rhs) {return lhs.m_parser_enum >= rhs.m_parser_enum;}
	friend bool operator< (const Method& lhs, const Method& rhs) {return lhs.m_parser_enum <  rhs.m_parser_enum;}
	friend bool operator<=(const Method& lhs, const Method& rhs) {return lhs.m_parser_enum <= rhs.m_parser_enum;}
	
	std::string Str() const;
	constexpr Enum Get() const {return m_parser_enum;}
	
private:
	Enum m_parser_enum;
};

#define XX(num, name, string) constexpr Method method_##name{num};
HTTP_METHOD_MAP(XX)
#undef XX

std::ostream& operator<<(std::ostream& os, Method method);

class Status
{
public:
	using Enum = ::http_status;
	
public:
	constexpr explicit Status(Enum parser_enum = static_cast<Enum>(HTTP_STATUS_OK)) : m_parser_enum{parser_enum} {}
	constexpr explicit Status(unsigned parser_enum) : m_parser_enum{static_cast<Enum>(parser_enum)} {}
	
	friend bool operator==(const Status& lhs, const Status& rhs) {return lhs.m_parser_enum == rhs.m_parser_enum;}
	friend bool operator!=(const Status& lhs, const Status& rhs) {return lhs.m_parser_enum != rhs.m_parser_enum;}
	friend bool operator> (const Status& lhs, const Status& rhs) {return lhs.m_parser_enum >  rhs.m_parser_enum;}
	friend bool operator>=(const Status& lhs, const Status& rhs) {return lhs.m_parser_enum >= rhs.m_parser_enum;}
	friend bool operator< (const Status& lhs, const Status& rhs) {return lhs.m_parser_enum <  rhs.m_parser_enum;}
	friend bool operator<=(const Status& lhs, const Status& rhs) {return lhs.m_parser_enum <= rhs.m_parser_enum;}
	
	std::string Str() const;
	constexpr Enum Get() const {return m_parser_enum;}
	
private:
	Enum m_parser_enum;
};

#define XX(num, name, string) constexpr Status status_##name{num};
HTTP_STATUS_MAP(XX)
#undef XX

std::ostream& operator<<(std::ostream& os, Status status);

} // end of namespace
