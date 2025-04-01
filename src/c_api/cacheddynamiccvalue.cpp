/*
 * cacheddynamicvalue.cpp
 *
 *  Created on: Oct 25, 2024
 *      Author: js
 */

#include <c_api/cacheddynamiccvalue.h>
#include <sstream>

namespace rhdl {

template <class Container, class ValueType>
CachedDynamicCValue<Container, ValueType>::CachedDynamicCValue(
		std::string name, Container &container,
		void (Container::*compute)(ValueType &result) const)
		: CValue(RHDL_CSTRING, name), cached_(container, compute)
{}

template <class Container, class ValueType>
CachedDynamicCValue<Container, ValueType>::~CachedDynamicCValue() {}

template<class Container, class ValueType>
const char* CachedDynamicCValue<Container, ValueType>::to_cstring() const
{
	return ((const std::string &) cached_()).c_str();
}

template class CachedDynamicCValue<Representation, std::string>;

} /* namespace rhdl */
