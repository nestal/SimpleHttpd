/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the booksrv
    distribution for more details.
*/

//
// Created by nestal on 11/8/17.
//

#include "FileUpload.hh"

#include "httpd/Response.hh"
#include "http-parser/http_parser.h"

namespace http {

FileUpload::FileUpload(const std::string& path) : m_file{path}
{
}

future<http::Response> FileUpload::OnContent(const char *data, std::size_t size)
{
	m_file.rdbuf()->sputn(data, size);
	return {};
}

future<http::Response> FileUpload::Finish()
{
	return BrightFuture::make_ready_future(http::Response{HTTP_STATUS_OK});
}

} // end of namespace
