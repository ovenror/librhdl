/*
 * simpleport.cpp
 *
 *  Created on: Nov 17, 2021
 *      Author: ovenror
 */

#include "simpleport.h"
#include "simpleconnection.h"

namespace rhdl {
namespace structural {
namespace builder {

SimplePort::SimplePort(Element &e, const ISingle &i)
	: ExistingPortBase<SimplePort>(e, i), iface_(i)
{
	new SimpleConnection(*this);

	c_ptr() -> iface = iface_.c_ptr();
}

SimplePort::~SimplePort() {}

void SimplePort::connectCompat(ExistingPort &peer)
{
	peer.connectCompat(*this);
}

void SimplePort::connectCompat(SimplePort &peer)
{
	ExistingPortBase<SimplePort>::connectCompat(peer);
}

} /* namespace builder */
} /* namespace structural */
} /* namespace rhdl */
