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

class IgnoreContent : public ContentHandler
{
public:
	explicit IgnoreContent(const Response& response) : m_response{response} {}
	explicit IgnoreContent(Response&& response) : m_response{std::move(response)} {}
	
	future<Response> OnContent(Request&, const char *, std::size_t) override
	{
		return {};
	}
	future<Response> Finish(Request&) override
	{
		return BrightFuture::make_ready_future(std::move(m_response));
	}

private:
	Response m_response;
};

ContentHandlerPtr ResponseWith(const Response& response)
{
	return std::make_unique<IgnoreContent>(response);
}

ContentHandlerPtr ResponseWith(Response&& response)
{
	return std::make_unique<IgnoreContent>(std::move(response));
}

class FileUpload : public ContentHandler
{
public:
	explicit FileUpload(const std::string& path, const Response& success, const Response& failed) :
		m_file{path},
		m_success{success},
		m_failed{failed}
	{
	}
	
	future<http::Response> OnContent(Request&, const char *data, std::size_t size) override
	{
		m_file.rdbuf()->sputn(data, size);
		return {};
	}
	future<http::Response> Finish(Request&) override
	{
		return BrightFuture::make_ready_future(
			std::move(m_file ? m_success : m_failed)
		);
	}

private:
	std::ofstream   m_file;
	Response m_success, m_failed;
};

ContentHandlerPtr SaveContentToFile(const std::string& path, const Response& success, const Response& failed)
{
	return std::make_unique<FileUpload>(path, success, failed);
}

RequestHandler::Function RequestHandler::Adapt(Response&& response)
{
	return [response=std::move(response)](Request&) mutable
	{
		return std::make_unique<IgnoreContent>(std::move(response));
	};
}

RequestHandler::Function RequestHandler::Adapt(const Response& response)
{
	return [response](Request&) mutable
	{
		return std::make_unique<IgnoreContent>(std::move(response));
	};
}

RequestHandler::Function RequestHandler::Adapt(Status status)
{
	return [status](Request&)
	{
		return std::make_unique<IgnoreContent>(Response{status});
	};
}

ContentHandlerPtr RequestHandler::operator()(Request& req) const
{
	return m_func(req);
}

} // end of namespace
