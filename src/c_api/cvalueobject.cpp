/*
 * ValueObject.cpp
 *
 *  Created on: Apr 5, 2025
 *      Author: js
 */

#include "c_api/cvalueobject.h"

namespace rhdl {

CValueObject::CValueObject(rhdl_type typeId, std::string name)
		: ComplexCObject(typeId, name) {}

CValueObject::CValueObject(CValueObject &&moved)
		: ComplexCObject(std::move(moved))
{
	Super::clearDict();
}

CValueObject::~CValueObject() {}

const CObject &CValueObject::add_after_move(const CValue &cvalue)
{
	return add(&cvalue);
}

} /* namespace rhdl */
