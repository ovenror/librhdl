/*
 * Component.cpp
 *
 *  Created on: Apr 4, 2021
 *      Author: ovenror
 */

#include <rhdl/construction/component.h>
#include "construction/library.h"

#include "representation/structural/builder/partialstructurebuilder.h"

namespace rhdl {

namespace sb = structural::builder;

Component::Component(const std::string &name) :
	Connector(sb::makeComponent(name))
{}


}
