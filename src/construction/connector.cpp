/*
 * port.cpp
 *
 *  Created on: Jul 12, 2021
 *      Author: ovenror
 */

#include <rhdl/construction/connector.h>
#include "representation/structural/builder/existingport.h"

namespace rhdl {

using structural::builder::Port;

Connector::Connector(Port &thePort) : ph_(thePort)
{
	assert (&thePort);
}

Connector::Connector(const Connector &c)
	: Connector(c.ph_.port())
{}

Connector::~Connector() {}

Connector Connector::operator [](const std::string &iname) const
{
	return Connector((ph_.port())[iname]);
}

} /* namespace rhdl */
