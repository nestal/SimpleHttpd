/*
	Copyright © 2016 Wan Wai Ho <me@nestal.net>
	All Rights Reserved.
*/

//
// Created by nestal on 5/22/16.
//

#include "UriString.hh"

namespace http {

boost::string_view UriString::operator[](std::size_t idx) const
{
	if (m_uri.empty() || m_uri.front() != '/')
		return "";

	auto pos = 1ULL;
	auto end = m_uri.find('/', pos);

	while (idx > 0 && end != m_uri.npos)
	{
		pos = end + 1;
		end = m_uri.find('/', pos);
		--idx;
	}

	return idx == 0 ? m_uri.substr(pos, end != m_uri.npos ? end - pos : m_uri.npos) : boost::string_view{};
}

std::string UriString::Str() const
{
	return m_uri.to_string();
}

std::ostream& operator<<(std::ostream& os, const UriString& uri)
{
	return os << uri.Str();
}

bool operator==(const UriString& left, const UriString& right)
{
	return left.Str() == right.Str();
}

} // end of namespace
