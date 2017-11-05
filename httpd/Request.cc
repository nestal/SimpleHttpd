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

http::Method Request::Method() const
{
	return m_method;
}

int Request::MajorVersion() const
{
	return m_major;
}

int Request::MinorVersion() const
{
	return m_minor;
}

const UriString& Request::Uri() const
{
	return m_uri;
}

const HeaderList& Request::Headers() const
{
	return m_headers;
}

const std::string& Request::Content() const
{
	return m_content;
}

void Request::OnMessageStart(http::Method method, std::string&& url, int major, int minor)
{
	m_method = method;
	m_uri.Str(std::move(url));
	m_major = major;
	m_minor = minor;
}

void Request::OnHeader(std::string&& field, std::string&& value)
{
	m_headers.Add({field, value});
}

int Request::OnContent(const char *data, std::size_t size)
{
	m_content.append(data, size);
	return 0;
}

int Request::OnMessageEnd()
{
	return 0;
}

int Request::OnHeaderComplete()
{
	return 0;
}
	
} // end of namespace
