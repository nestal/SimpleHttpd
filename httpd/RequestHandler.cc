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
#include <iostream>

namespace http {

class FileUpload : public ContentHandler
{
public:
	explicit FileUpload(const std::string& path, UploadCallback&& response) :
		m_file{path, std::ios::in|std::ios::out|std::ios::trunc},
		m_response{std::move(response)}
	{
		if (!m_file)
			std::cerr << "cannot open " << path << "! " << strerror(errno) << std::endl;
	}
	
	boost::optional<http::Response> OnContent(Request&, const char *data, std::size_t size) override
	{
		m_file.rdbuf()->sputn(data, size);
		return {};
	}
	boost::optional<http::Response> ReplyNow(Request& req) override
	{
		return m_response(req, m_file);
	}
	
	future<Response> ReplyLater(Request& request) override
	{
		return {};
	}

private:
	std::fstream    m_file;
	UploadCallback  m_response;
};

ContentHandlerPtr SaveContentToFile(const std::string& path, UploadCallback&& response)
{
	return std::make_unique<FileUpload>(path, std::move(response));
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
