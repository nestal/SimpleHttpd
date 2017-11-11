/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SimpleHttp
    distribution for more details.
*/

//
// Created by nestal on 11/8/17.
//

#include "RequestHandler.hh"

#include <fstream>

namespace http {

class FileUpload : public ContentHandler
{
public:
	explicit FileUpload(const std::string& path, const Response& success, const Response& failed) :
		m_file{path},
		m_success{success},
		m_failed{failed}
	{
	}
	
	boost::optional<http::Response> OnContent(Request&, const char *data, std::size_t size) override
	{
		m_file.rdbuf()->sputn(data, size);
		return {};
	}
	boost::optional<http::Response> ReplyNow(Request&) override
	{
		return m_file ? m_success : m_failed;
	}
	
	future<Response> ReplyLater(Request& request) override
	{
		return {};
	}

private:
	std::ofstream   m_file;
	Response m_success, m_failed;
};

ContentHandlerPtr SaveContentToFile(const std::string& path, const Response& success, const Response& failed)
{
	return std::make_unique<FileUpload>(path, success, failed);
}

Response RequestHandler::operator()(Request& req) const
{
	return m_func(req);
}

RequestHandler::Function RequestHandler::Adapt(Response&& response)
{
	return [response=std::move(response)](Request&){return response;};
}

RequestHandler::Function RequestHandler::Adapt(const Response& response)
{
	return [response](Request&){return response;};
}

RequestHandler::Function RequestHandler::Adapt(Status status)
{
	return [status](Request&){return Response{status};};
}

} // end of namespace
