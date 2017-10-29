/*
	Copyright © 2016 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SongBits
    distribution for more details.
*/

//
// Created by nestal on 5/11/16.
//

#pragma once

#include "BrightFuture.hh"
#include <boost/asio.hpp>

#include <unordered_set>
#include <memory>

namespace http {

class RequestDispatcher;

class ConnectionManager
{
private:
	class Entry;
	
public:
	ConnectionManager(BrightFuture::Executor *exec);

	void Start(
		boost::asio::ip::tcp::socket&&  sock,
		const RequestDispatcher&        handler
	);

	void StopAll();

private:
	using EntryPtr = std::shared_ptr<Entry>;
	void Stop(const EntryPtr& p);

private:
	std::unordered_set<EntryPtr> m_conn;
	BrightFuture::Executor       *m_exec;
};

} // end of namespace
