/*
 * ValueObject.cpp
 *
 *  Created on: Oct 13, 2024
 *      Author: js
 */

#include "c_api/valueobject.h"
#include "c_api/cvalue.h"


namespace rhdl {

template <class C_Struct, enum rhdl_type TYPE>
ValueObject<C_Struct, TYPE>::ValueObject(std::string name) : Super(name)
{}

template <class C_Struct, enum rhdl_type TYPE>
ValueObject<C_Struct, TYPE>::ValueObject(ValueObject &&moved) : Super(std::move(moved))
{
	Super::clearDict();
}

template <class C_Struct, enum rhdl_type TYPE>
ValueObject<C_Struct, TYPE>::~ValueObject() {}

template class ValueObject<rhdl_representation, RHDL_REPRESENTATION>;
template class ValueObject<rhdl_transformation, RHDL_TRANSFORMATION>;

} /* namespace rhdl */
