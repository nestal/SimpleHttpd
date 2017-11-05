/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the SimpleHttp
    distribution for more details.
*/

//
// Created by nestal on 11/5/17.
//

#pragma once

#include "BrightFuture/executor/BoostAsioExecutor.hh"

namespace http {

using BrightFuture::future;
using BrightFuture::promise;
using Executor = BrightFuture::BoostAsioExecutor;

}