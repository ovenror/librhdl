/*
 * cachednostructcvalue.cpp
 *
 *  Created on: Oct 25, 2024
 *      Author: js
 */

#include <c_api/cachednostructcvalue.h>
#include <sstream>

namespace rhdl {

template <class Container, class ValueType>
CachedNoStructCValue<Container, ValueType>::CachedNoStructCValue(
		std::string name, Container &container,
		void (Container::*compute)(ValueType &result) const)
		: CValue(RHDL_CSTRING, name), cached_(container, compute)
{}

template <class Container, class ValueType>
CachedNoStructCValue<Container, ValueType>::~CachedNoStructCValue() {}

template<class Container, class ValueType>
const char* CachedNoStructCValue<Container, ValueType>::to_cstring() const
{
	return ((const std::string &) cached_()).c_str();
}

template class CachedNoStructCValue<Representation, std::string>;

} /* namespace rhdl */
