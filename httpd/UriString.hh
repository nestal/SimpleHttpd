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
	
	template <typename T>
	UriString(T&& uri) : m_uri{std::forward<T>(uri)}
	{
	}

	std::string operator[](std::size_t idx) const;

	const std::string& Str() const;
	
	template <typename T>
	void Str(T&& str)
	{
		m_uri = std::forward<T>(str);
	}

private:
	std::string m_uri;
};

std::ostream& operator<<(std::ostream& os, const UriString& uri);

bool operator==(const UriString& left, const UriString& right);

} // end of namespace
