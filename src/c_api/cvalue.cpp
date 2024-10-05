/*
 * cvalue.cpp
 *
 *  Created on: Oct 3, 2024
 *      Author: ovenror
 */

#include <rhdl/construction/c/types.h>
#include "c_api/cvalue.h"
#include <cassert>

namespace rhdl {

CValue::CValue(rhdl_type typeId, std::string name)
		: CObject(typeId, name)
{}

CValue::~CValue() {}

} /* namespace rhdl */
