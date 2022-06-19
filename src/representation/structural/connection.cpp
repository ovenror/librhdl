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
{}

std::ostream& operator <<(std::ostream &os, Connection &c)
{
	c.toStream(os);
	return os;
}

} /* namespace rhdl::structural */
