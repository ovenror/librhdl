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
{
	c_strings_.push_back(0);
	setMembers();
}

CValue::CValue(CValue &&moved) : CObject(std::move(moved))
{
	c_strings_.push_back(0);
	setMembers();
}


CValue::~CValue() {}

const CObject& CValue::at(const std::string &name) const
{
	throw ConstructionException(Errorcode::E_NO_SUCH_MEMBER);
}

const CObject& CValue::at(const char *name) const
{
	throw ConstructionException(Errorcode::E_NO_SUCH_MEMBER);
}

} /* namespace rhdl */
