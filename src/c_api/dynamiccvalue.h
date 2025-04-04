/*
 * DynamicCValue.h
 *
 *  Created on: Oct 14, 2024
 *      Author: js
 */

#ifndef SRC_C_API_DYNAMICCVALUE_H_
#define SRC_C_API_DYNAMICCVALUE_H_

#include "typedcvalue.h"
#include "util/cached.h"

#include <type_traits>

namespace rhdl {

template <class VALUE_TYPE>
class DynamicCValue : public TypedCValue<VALUE_TYPE, true>
{
	using Super = TypedCValue<VALUE_TYPE, true>;

public:
	DynamicCValue(std::string name, CValueContainer &container)
		: Super(name, container) {}
	DynamicCValue(DynamicCValue &&moved, CValueContainer &newContainer)
		: Super(std::move(moved), newContainer) {}

	virtual ~DynamicCValue() {}

	operator typename Super::ReturnTypeNoStringRef() const override {return value();}

protected:
	virtual VALUE_TYPE value() = 0;
};

} /* namespace rhdl */

#endif /* SRC_C_API_DYNAMICCVALUE_H_ */
