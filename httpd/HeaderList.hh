/*
	Copyright © 2016 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SongBits
    distribution for more details.
*/



//
// Created by nestal on 5/12/16.
//

#pragma once

#include <string>
#include <vector>

#include <boost/optional.hpp>

namespace http {

class HeaderList
{
public:
	struct Header
	{
		std::string name;
		std::string value;
	};

	using iterator = std::vector<Header>::const_iterator;

public:
	HeaderList() = default;
	HeaderList(const HeaderList&) = default;
	HeaderList& operator=(const HeaderList&) = default;

	const std::string& Field(const std::string& name) const;

	// generic field access
	void Add(Header&& hdr);
	bool Empty() const;
	std::size_t Count() const;

	iterator begin() const;
	iterator end() const;

private:
	std::vector<Header> m_;
};

} // end of namespace
