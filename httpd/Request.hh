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

class RequestCallback
{
public:
	virtual ~RequestCallback() = default;
	
	virtual void OnMessageStart(std::string&& method, std::string&& url, int major, int minor) = 0;
	virtual void OnHeader(std::string&& field, std::string&& value) = 0;
	virtual void OnContent(const char *data, std::size_t size) = 0;
	virtual void OnMessageEnd() = 0;
};

/**
 * \brief   HTTP Request
 */
class Request : public RequestCallback
{
public:
	Request() = default;

	void OnMessageStart(std::string&& method, std::string&& url, int major, int minor) override;
	void OnHeader(std::string&& field, std::string&& value) override;
	void OnContent(const char *data, std::size_t size) override;
	void OnMessageEnd() override;
	
	const std::string& Method() const;
	int MajorVersion() const;
	int MinorVersion() const;
	const UriString& Uri() const;

	const HeaderList& Headers() const;
	const std::string& Content() const;
	
private:
	std::string m_method;
	UriString   m_uri;
	
	int m_major = 0;
	int m_minor = 0;

	HeaderList m_headers;

	std::string m_content;
};

} // end of namespace
