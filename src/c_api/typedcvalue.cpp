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

template <class VALUE_TYPE>
TypedCValue<VALUE_TYPE>::~TypedCValue() {}

template<class VALUE_TYPE>
inline const char* TypedCValue<VALUE_TYPE>::to_cstring() const
{
	return (std::stringstream() << value_).str().c_str();
}

template<>
inline const char* TypedCValue<rhdl_direction>::to_cstring() const
{
	return (std::stringstream() << SingleDirection(value_)).str().c_str();
}

template class TypedCValue<const char *>;
template class TypedCValue<int64_t>;
template class TypedCValue<uint64_t>;
template class TypedCValue<rhdl_direction>;

} /* namespace rhdl */
