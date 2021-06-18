/*
 * structureconnector.cpp
 *
 *  Created on: Jul 12, 2021
 *      Author: ovenror
 */

#include <rhdl/construction/structureconnector.h>
#include <rhdl/construction/constructionexception.h>

#include "representation/structural/builder/port.h"

namespace rhdl {

StructureConnector::StructureConnector(structural::builder::Port &port)
	: Connector(port)
{}

StructureConnector StructureConnector::operator [](const std::string &iname) const
{
	return StructureConnector(ph_.port()[iname]);
}

} /* namespace rhdl */
