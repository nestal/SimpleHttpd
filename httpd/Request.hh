/*
	Copyright © 2016 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SongBits
    distribution for more details.
*/

//
// Created by nestal on 5/11/16.
//

#pragma once

#include "HeaderList.hh"
#include "UriString.hh"

#include <boost/asio/ip/tcp.hpp>

#include <string>

namespace http {

/**
 * \brief   HTTP Request
 */
class Request
{
public:
	Request() = default;

	void SetMethod(const std::string& method);
	const std::string& Method() const;

	void SetMajorVersion(int version);
	void SetMinorVersion(int version);
	int MajorVersion() const;
	int MinorVersion() const;

	void SetUri(const std::string& uri);
	const UriString& Uri() const;

	void AddHeader(HeaderList::Header&& hdr);
	const HeaderList& Headers() const;

	const std::string& Content() const;
	void SetContent(std::string&& content);

private:
	std::string m_method;
	UriString   m_uri;
	
	int m_major = 0;
	int m_minor = 0;

	HeaderList m_headers;

	std::string m_content;
};

} // end of namespace
