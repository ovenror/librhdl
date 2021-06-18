/*
 * handle.cpp
 *
 *  Created on: Apr 6, 2021
 *      Author: ovenror
 */

#include "handle.h"
#include "context.h"

#include "representation/structural/builder/port.h"

namespace rhdl {

using structural::builder::Port;

Handle::Handle(Context& context, Port &port)
	: context_(context), ph_(port), c_(*this)
{
	updateC();
}

Handle::~Handle() {}

void Handle::pt() const
{
	throw ConstructionException(Errorcode::E_ILLEGAL_PASSTHROUGH);
}

Handle& Handle::select(const std::string &name) const
{
	return context_.make(ph_.port()[name]);
}

void Handle::connect(Handle &h)
{
	auto &lhs = ph_.port();
	auto &rhs = h.ph_.port();

	if (lhs.isExternal() && rhs.isExternal())
		pt();

	lhs.connectTo(rhs);

	auto &newlhs = ph_.port();
	auto &newrhs = h.ph_.port();

	if (&newlhs != &lhs)
		updateC();

	if (&newrhs != &rhs)
		h.updateC();

}

void Handle::updateC()
{
	c_ptr(*this) -> iface = ph_.port().c_ptr_iface();
}

}

