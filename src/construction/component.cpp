/*
 * Component.cpp
 *
 *  Created on: Apr 4, 2021
 *      Author: ovenror
 */

#include <rhdl/construction/component.h>
#include "construction/parthandle.h"

namespace rhdl {

Component::Component(const std::string &name) :
		handle_(std::make_unique<PartHandle>(name)) {}

Component::~Component() {
}

PartInterface Component::operator [](const std::string& iname) const
{
	return (*handle_)[iname];
}

const Interfacible& Component::interfacible() const
{
	return *handle_;
}

}
