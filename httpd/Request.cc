/*
	Copyright © 2016 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SongBits
    distribution for more details.
*/


//
// Created by nestal on 5/11/16.
//

#include "Request.hh"

namespace http {

using namespace boost::asio;

void Request::SetMethod(const std::string& method)
{
	m_method = method;
}

const std::string& Request::Method() const
{
	return m_method;
}

void Request::SetMajorVersion(int version)
{
	m_major = version;
}

void Request::SetMinorVersion(int version)
{
	m_minor = version;
}

int Request::MajorVersion() const
{
	return m_major;
}

int Request::MinorVersion() const
{
	return m_minor;
}

void Request::SetUri(const std::string& uri)
{
	m_uri = uri;
}

const UriString& Request::Uri() const
{
	return m_uri;
}

void Request::AddHeader(HeaderList::Header&& hdr)
{
	m_headers.Add(std::move(hdr));
}

const HeaderList& Request::Headers() const
{
	return m_headers;
}

const std::string& Request::Content() const
{
	return m_content;
}

void Request::SetContent(std::string&& content)
{
	m_content = std::move(content);
}
	
} // end of namespace
