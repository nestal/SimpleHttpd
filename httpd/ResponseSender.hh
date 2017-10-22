/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the simpleWeb
    distribution for more details.
*/

//
// Created by nestal on 4/5/17.
//

#pragma once

#include <string>

namespace http {

enum class ResponseStatus;

class ResponseStatusSender;
class ResponseHeaderSender;
class ResponseContentSender;

class ResponseStatusSender
{
public:
	virtual ResponseHeaderSender* SendStatus(ResponseStatus status) = 0;
};

class ResponseHeaderSender
{
public:
	virtual ~ResponseHeaderSender() = default;
	
	virtual ResponseHeaderSender* SendHeader(const std::string& header, const std::string& value) = 0;
	virtual ResponseContentSender* FinishSendingHeader() = 0;
};

class ResponseContentSender
{
public:
	virtual ~ResponseContentSender() = default;
	
	virtual ResponseContentSender* SendContent(const ) = 0;
	virtual ResponseContentSender* FinishSendingHeader() = 0;
	
};

} // end of namespace
