/*
 * extendedstructure.cpp
 *
 *  Created on: Jul 28, 2021
 *      Author: ovenror
 */

#include "construction/internalstructure.h"

namespace rhdl {

InternalStructure::InternalStructure(const std::string &name, Mode mode, Namespace &ns)
	: Structure(name, mode, &ns)
{}

} /* namespace rhdl */
