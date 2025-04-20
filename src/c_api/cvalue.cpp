/*
 * cvalue.cpp
 *
 *  Created on: Oct 3, 2024
 *      Author: ovenror
 */

#include "rhdl/construction/c/types.h"
#include "cvalue.h"
#include "complexcobject.h"
#include <cassert>

namespace rhdl {

CValue::CValue(rhdl_type typeId, std::string name, CValueContainer &container)
		: CObject(typeId, name)
{
	container.add(*this);
}

CValue::CValue(CValue &&moved, CValueContainer &newContainer)
		: CObject(std::move(moved))
{
	newContainer.add_after_move(*this);
}

CValue::~CValue() {}

} /* namespace rhdl */
