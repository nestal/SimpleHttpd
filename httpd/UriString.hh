/*
	Copyright © 2016 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SongBits
    distribution for more details.
*/

//
// Created by nestal on 5/22/16.
//

#pragma once

#include <boost/utility/string_view.hpp>
#include <string>
#include <iosfwd>

namespace http {

/**
 * \brief Simple wrapper for URIs
 *
 * Use operator[] to get subpaths of the URI.
 */
class UriString
{
public:
	UriString() = default;
	UriString(const UriString&) = default;
	UriString(UriString&&) = default;
	UriString& operator=(const UriString&) = default;
	UriString& operator=(UriString&&) = default;
	
	UriString(const std::string& uri) : m_uri{uri}
	{
	}
	
	UriString(boost::string_view uri) : m_uri{uri}
	{
	}

	template <std::size_t n>
	UriString(const char (&str)[n]) : m_uri{str}
	{
	}
	
	boost::string_view operator[](std::size_t idx) const;

	std::string Str() const;
	
private:
	boost::string_view m_uri;
};

std::ostream& operator<<(std::ostream& os, const UriString& uri);

bool operator==(const UriString& left, const UriString& right);

} // end of namespace
