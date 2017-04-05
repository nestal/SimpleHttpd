/*
	Copyright © 2016 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SongBits
    distribution for more details.
*/


//
// Created by nestal on 5/14/16.
//

#include "HeaderList.hh"

#include <boost/algorithm/string.hpp>
#include <algorithm>

namespace http {

const std::string& HeaderList::Field(const std::string& name) const
{
	static const std::string empty;
	auto it = std::find_if(
		m_.begin(),
		m_.end(),
		[name](const Header& h)
		{
			return boost::iequals(h.name, name);
		}
	);
	return it == m_.end() ? empty : it->value;
}

void HeaderList::Add(Header&& hdr)
{
	m_.push_back(std::move(hdr));
}

bool HeaderList::Empty() const
{
	return m_.empty();
}

HeaderList::iterator HeaderList::begin() const
{
	return m_.begin();
}

HeaderList::iterator HeaderList::end() const
{
	return m_.end();
}

std::size_t HeaderList::ContentLength() const
{
	if (!m_content_length)
	{
		auto& str = Field("Content-Length");
		m_content_length = (str.empty() ? 0 : std::stoull(str));
	}

	return m_content_length.get();
}

} // end of namespace
