/*
	Copyright © 2016 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SongBits
    distribution for more details.
*/



//
// Created by nestal on 5/15/16.
//

#include "QueryString.hh"

namespace http {

QueryString::QueryString(std::string&& str) : m_str(std::move(str))
{
}

std::string QueryString::Field(std::string name) const
{
	name.push_back('=');

	auto pos = m_str.find(name);
	if (pos != m_str.npos)
	{
		auto epos = m_str.find('&', pos + name.size());
		auto spos = pos + name.size();
		return m_str.substr(spos, epos - spos);
	}

	return "";
}

} // end of namespace
