/*
	Copyright © 2016 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SongBits
    distribution for more details.
*/

//
// Created by nestal on 5/14/16.
//

#include "Response.hh"

namespace http {

using namespace boost::asio;

namespace status_strings {

const std::string ok =
	"HTTP/1.0 200 OK\r\n";
const std::string created =
	"HTTP/1.0 201 Created\r\n";
const std::string accepted =
	"HTTP/1.0 202 Accepted\r\n";
const std::string no_content =
	"HTTP/1.0 204 No Content\r\n";
const std::string multiple_choices =
	"HTTP/1.0 300 Multiple Choices\r\n";
const std::string moved_permanently =
	"HTTP/1.0 301 Moved Permanently\r\n";
const std::string moved_temporarily =
	"HTTP/1.0 302 Moved Temporarily\r\n";
const std::string not_modified =
	"HTTP/1.0 304 Not Modified\r\n";
const std::string bad_request =
	"HTTP/1.0 400 Bad Request\r\n";
const std::string unauthorized =
	"HTTP/1.0 401 Unauthorized\r\n";
const std::string forbidden =
	"HTTP/1.0 403 Forbidden\r\n";
const std::string not_found =
	"HTTP/1.0 404 Not Found\r\n";
const std::string internal_server_error =
	"HTTP/1.0 500 Internal Server Error\r\n";
const std::string not_implemented =
	"HTTP/1.0 501 Not Implemented\r\n";
const std::string bad_gateway =
	"HTTP/1.0 502 Bad Gateway\r\n";
const std::string service_unavailable =
	"HTTP/1.0 503 Service Unavailable\r\n";

const_buffer ToBuffer(ResponseStatus status)
{
	switch (status)
	{
	case ResponseStatus::ok:
		return buffer(ok);
	case ResponseStatus::created:
		return buffer(created);
	case ResponseStatus::accepted:
		return buffer(accepted);
	case ResponseStatus::no_content:
		return buffer(no_content);
	case ResponseStatus::multiple_choices:
		return buffer(multiple_choices);
	case ResponseStatus::moved_permanently:
		return buffer(moved_permanently);
	case ResponseStatus::moved_temporarily:
		return buffer(moved_temporarily);
	case ResponseStatus::not_modified:
		return buffer(not_modified);
	case ResponseStatus::bad_request:
		return buffer(bad_request);
	case ResponseStatus::unauthorized:
		return buffer(unauthorized);
	case ResponseStatus::forbidden:
		return buffer(forbidden);
	case ResponseStatus::not_found:
		return buffer(not_found);
	case ResponseStatus::not_implemented:
		return buffer(not_implemented);
	case ResponseStatus::bad_gateway:
		return buffer(bad_gateway);
	case ResponseStatus::service_unavailable:
		return buffer(service_unavailable);
	
	case ResponseStatus::internal_server_error:
		break;
	}
	
	return buffer(internal_server_error);
}

} // namespace status_strings

namespace misc_strings {

const char name_value_separator[] = { ':', ' ' };
const char crlf[] = { '\r', '\n' };

} // namespace misc_strings

std::vector<const_buffer> Response::ToBuffers()
{
	// construct Content-Length header with the actual length of content
	m_content_length = "Content-Length: " + std::to_string(m_content.size()) + "\r\n";
	
	return std::vector<const_buffer>{
		status_strings::ToBuffer(m_status),
		buffer(m_content_type),
		buffer(m_content_length),
		buffer(m_other_headers),
		buffer(misc_strings::crlf),
		buffer(m_content)
	};
}

namespace stock_replies {

const char ok[] =
	"<html>"
		"<head><title>OK!!</title></head>"
		"<body><h1>Request success</h1><p>Hello world!</p></body>"
		"</html>";
const char created[] =
	"<html>"
		"<head><title>Created</title></head>"
		"<body><h1>201 Created</h1></body>"
		"</html>";
const char accepted[] =
	"<html>"
		"<head><title>Accepted</title></head>"
		"<body><h1>202 Accepted</h1></body>"
		"</html>";
const char no_content[] =
	"<html>"
		"<head><title>No Content</title></head>"
		"<body><h1>204 Content</h1></body>"
		"</html>";
const char multiple_choices[] =
	"<html>"
		"<head><title>Multiple Choices</title></head>"
		"<body><h1>300 Multiple Choices</h1></body>"
		"</html>";
const char moved_permanently[] =
	"<html>"
		"<head><title>Moved Permanently</title></head>"
		"<body><h1>301 Moved Permanently</h1></body>"
		"</html>";
const char moved_temporarily[] =
	"<html>"
		"<head><title>Moved Temporarily</title></head>"
		"<body><h1>302 Moved Temporarily</h1></body>"
		"</html>";
const char not_modified[] =
	"<html>"
		"<head><title>Not Modified</title></head>"
		"<body><h1>304 Not Modified</h1></body>"
		"</html>";
const char bad_request[] =
	"<html>"
		"<head><title>Bad Request</title></head>"
		"<body><h1>400 Bad Request</h1></body>"
		"</html>";
const char unauthorized[] =
	"<html>"
		"<head><title>Unauthorized</title></head>"
		"<body><h1>401 Unauthorized</h1></body>"
		"</html>";
const char forbidden[] =
	"<html>"
		"<head><title>Forbidden</title></head>"
		"<body><h1>403 Forbidden</h1></body>"
		"</html>";
const char not_found[] =
	"<html>"
		"<head><title>Not Found</title></head>"
		"<body><h1>404 Not Found</h1></body>"
		"</html>";
const char internal_server_error[] =
	"<html>"
		"<head><title>Internal Server Error</title></head>"
		"<body><h1>500 Internal Server Error</h1></body>"
		"</html>";
const char not_implemented[] =
	"<html>"
		"<head><title>Not Implemented</title></head>"
		"<body><h1>501 Not Implemented</h1></body>"
		"</html>";
const char bad_gateway[] =
	"<html>"
		"<head><title>Bad Gateway</title></head>"
		"<body><h1>502 Bad Gateway</h1></body>"
		"</html>";
const char service_unavailable[] =
	"<html>"
		"<head><title>Service Unavailable</title></head>"
		"<body><h1>503 Service Unavailable</h1></body>"
		"</html>";

std::string to_string(ResponseStatus status)
{
	switch (status)
	{
	case ResponseStatus::ok:
		return ok;
	case ResponseStatus::created:
		return created;
	case ResponseStatus::accepted:
		return accepted;
	case ResponseStatus::no_content:
		return no_content;
	case ResponseStatus::multiple_choices:
		return multiple_choices;
	case ResponseStatus::moved_permanently:
		return moved_permanently;
	case ResponseStatus::moved_temporarily:
		return moved_temporarily;
	case ResponseStatus::not_modified:
		return not_modified;
	case ResponseStatus::bad_request:
		return bad_request;
	case ResponseStatus::unauthorized:
		return unauthorized;
	case ResponseStatus::forbidden:
		return forbidden;
	case ResponseStatus::not_found:
		return not_found;
	case ResponseStatus::internal_server_error:
		return internal_server_error;
	case ResponseStatus::not_implemented:
		return not_implemented;
	case ResponseStatus::bad_gateway:
		return bad_gateway;
	case ResponseStatus::service_unavailable:
		return service_unavailable;
	default:
		return internal_server_error;
	}
}

} // namespace stock_replies

void Response::SetStatus(ResponseStatus s)
{
	m_status = s;
}

void Response::SetContentType(const std::string& type)
{
	m_content_type = "Content-Type: " + type + "\r\n";
}

void Response::AddHeader(const std::string& header, const std::string& value)
{
	m_other_headers += (header + ": " + value + "\r\n");
}

void Response::SetContent(std::vector<char>&& buf)
{
	m_content = std::move(buf);
}
	
} // end of namespace
