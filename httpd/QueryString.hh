/*
	Copyright © 2016 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SongBits
    distribution for more details.
*/


//
// Created by nestal on 5/15/16.
//

#pragma once

#include <string>

namespace http {

class QueryString
{
public:
	QueryString(std::string&& str);

	std::string Field(std::string name) const;

private:
	std::string m_str;
};

} // end of namespace
