/*
 * cvalue.cpp
 *
 *  Created on: Oct 3, 2024
 *      Author: ovenror
 */

#include "complexcobject.h"
#include "rhdl/construction/c/types.h"
#include "util/emptydictionary.h"
#include "cvalue.h"
#include <cassert>

namespace rhdl {

CValue::CValue(rhdl_type typeId, std::string name, CValueContainer &container)
		: CObject(typeId, name)
{
	setDictionary(EmptyDictionary<CObject>());
	container.add(*this);
}

CValue::CValue(CValue &&moved, CValueContainer &newContainer)
		: CObject(std::move(moved))
{
	setDictionary(EmptyDictionary<CObject>());
	newContainer.add_after_move(*this);
}

CValue::~CValue() {}

} /* namespace rhdl */
