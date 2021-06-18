/*
 * porthandle.cpp
 *
 *  Created on: Jun 5, 2022
 *      Author: ovenror
 */

#include <rhdl/construction/porthandle.h>
#include "representation/structural/builder/existingport.h"

namespace rhdl {

PortHandle::PortHandle(structural::builder::Port &p)
	: port_(&p)
{
	port_ -> registerHandle(this);
}

PortHandle::~PortHandle()
{
	if (port_)
		port_ -> removeHandle(this);
}

void PortHandle::invalidate()
{
	port_ = nullptr;
}

void PortHandle::realizePort(structural::builder::ExistingPort &p)
{
	port_ = &p;
}

structural::builder::Port &PortHandle::port() const
{
	if (!port_)
		throw ConstructionException(Errorcode::E_INVALID_HANDLE);

	return *port_;
}

} /* namespace rhdl */
