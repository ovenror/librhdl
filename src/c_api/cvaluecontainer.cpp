/*
 * cvaluecontainer.cpp
 *
 *  Created on: Apr 8, 2025
 *      Author: js
 */

#include <c_api/cvaluecontainer.h>

namespace rhdl {

CValueContainer::CValueContainer(rhdl_type typeId, std::string name)
		: CObject(typeId, name) {}

CValueContainer::CValueContainer(CValueContainer &&moved)
		: CObject(std::move(moved)) {}

CValueContainer::~CValueContainer() {}

} /* namespace rhdl */
