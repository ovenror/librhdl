/*
 * representationtype.cpp
 *
 *  Created on: Jun 4, 2023
 *      Author: ovenror
 */

#include "representationtype.h"

namespace rhdl {

RepresentationType::RepresentationType(ID id, std::string name)
		: id_(id), name_(name)
{}

RepresentationType::~RepresentationType() {}

} /* namespace rhdl */
