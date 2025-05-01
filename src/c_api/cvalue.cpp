/*
 * cvalue.cpp
 *
 *  Created on: Oct 3, 2024
 *      Author: ovenror
 */

#include <util/dictionary/emptydictionary.h>
#include "complexcobject.h"
#include "rhdl/construction/c/types.h"
#include "cvalue.h"
#include <cassert>

namespace rhdl {

CValue::CValue(rhdl_type typeId, std::string name, CValueContainer &container)
		: CObject(typeId, name)
{
	setDictionary(dictionary::EmptyDictionary<const CObject>());
	container.add(*this);
}

CValue::CValue(CValue &&moved, CValueContainer &newContainer)
		: CObject(std::move(moved))
{
	setDictionary(dictionary::EmptyDictionary<const CObject>());
	newContainer.add_after_move(*this);
}

CValue::~CValue() {}

} /* namespace rhdl */
