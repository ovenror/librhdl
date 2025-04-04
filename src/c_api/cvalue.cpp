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
	setMembers();
	container.add(*this);
}

CValue::CValue(CValue &&moved, CValueContainer &newContainer)
		: CObject(std::move(moved)), c_strings_(std::move(moved.c_strings_))
{
	setMembers();
	newContainer.add_after_move(*this);
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
