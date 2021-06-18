/*
 * port.cpp
 *
 *  Created on: May 28, 2022
 *      Author: ovenror
 */

#include "port.h"
#include "interface/isingle.h"

namespace rhdl {
namespace structural {

Port::Port(ElementIdx e, const ISingle &i)
	: element_(e), iface_(i)
{
}

std::ostream& operator <<(std::ostream &os, const Port &p)
{
	os << "(#" << p.element() << ")[" << p.iface() << "]";
	return os;
}


} /* namespace structural */
} /* namespace rhdl */
