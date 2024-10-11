/*
 * cvalue.cpp
 *
 *  Created on: Oct 3, 2024
 *      Author: ovenror
 */

#include "c_api/typedcvalue.h"
#include "interface/direction.h"

namespace rhdl {

template <class VALUE_TYPE>
TypedCValue<VALUE_TYPE>::TypedCValue(std::string name, const VALUE_TYPE &value)
		: Super(name), value_(value)
{}

template<class VALUE_TYPE>
TypedCValue<VALUE_TYPE>::TypedCValue(TypedCValue &&moved)
		: Super(std::move(moved)), value_(moved.value_)
{}

template<class VALUE_TYPE>
TypedCValue<VALUE_TYPE>::TypedCValue(const TypedCValue &lol)
		: Super(lol.name()), value_(lol.value_)
{}

template <class VALUE_TYPE>
TypedCValue<VALUE_TYPE>::~TypedCValue() {}

template<class VALUE_TYPE>
const char* TypedCValue<VALUE_TYPE>::to_cstring() const
{
	return (std::stringstream() << value_).str().c_str();
}

template<>
const char* TypedCValue<rhdl_direction>::to_cstring() const
{
	return (std::stringstream() << SingleDirection(value_)).str().c_str();
}

template class TypedCValue<const char *>;
template class TypedCValue<int64_t>;
template class TypedCValue<uint64_t>;
template class TypedCValue<rhdl_direction>;
template class TypedCValue<rhdl_reptype>;

} /* namespace rhdl */
