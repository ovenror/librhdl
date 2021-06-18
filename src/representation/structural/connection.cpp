/*
 * flatconnection.cpp
 *
 *  Created on: Jun 14, 2021
 *      Author: ovenror
 */

#include <representation/structural/connection.h>
#include "interface/isingle.h"

#include "util/catiterator.h"

#include <algorithm>
#include <array>
#include <tuple>
#include <vector>

namespace rhdl::structural {

Connection::Connection(std::vector<Port> ports)
	: ConnectionBase(std::move(ports))
{
	computeClosings();
}

static bool isInternalOpen(const Port &port)
{
	return !port.isExternal() && port.iface().is_open();
}

/* If there are open internal ports
 *   1. Set all external interfaces to open
 *   2. Close all internal interfaces but one
 */
void Connection::computeClosings()
{
	auto nOpen = std::count_if(begin(), end(), isInternalOpen);

	if (nOpen == 0)
		return;

	for (auto &port : container_) {
		if (port.isExternal()) {
			port.iface().setOpen();
			return;
		}

		if (nOpen <= 1)
			return;

		port.close();
		--nOpen;
	};
}

std::ostream& operator <<(std::ostream &os, Connection &c)
{
	c.toStream(os);
	return os;
}

} /* namespace rhdl::structural */
