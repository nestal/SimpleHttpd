/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the booksrv
    distribution for more details.
*/

//
// Created by nestal on 11/8/17.
//

#pragma once

#include "httpd/RequestHandler.hh"
#include <fstream>

namespace http {

class FileUpload : public ContentHandler
{
public:
	FileUpload(const std::string& path);
	
	future<http::Response> OnContent(Request&, const char *data, std::size_t size) override;
	future<http::Response> Finish(Request&) override;

private:
	std::ofstream   m_file;
};

} // end of namespace
