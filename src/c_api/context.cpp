/*
 * unsafecontext.cpp
 *
 *  Created on: Apr 6, 2021
 *      Author: ovenror
 */

#include "context.h"
#include "handle.h"

#include <cassert>

namespace rhdl {

Context::~Context() {
	for (const Handle *handle : handles_)
		delete handle;
}

bool Context::contains(const Handle* handle) const {
	return handles_.find(handle) != handles_.end();
}

void Context::checkContains(const Handle* handle) const {
	if (!contains(handle))
		throw Errorcode::E_UNKNOWN_STRUCT;
}

Handle &Context::make(structural::builder::Port &port)
{
	auto *h = new Handle(*this, port);
	handles_.insert(h);
	return *h;
}

} /* namespace rhdl */
