/*
 * simpleconnection.cpp
 *
 *  Created on: May 28, 2022
 *      Author: ovenror
 */

#include "simpleconnection.h"
#include "connectionsbuilder.h"

namespace rhdl {
namespace structural {
namespace builder {

SimpleConnection::~SimpleConnection() {}

void SimpleConnection::build(ConnectionsBuilder &b) const
{
	b.build(*this);
}

} /* namespace builder */
} /* namespace structural */
} /* namespace rhdl */
