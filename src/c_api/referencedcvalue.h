/*
 * storedcvalue.h
 *
 *  Created on: Apr 5, 2025
 *      Author: js
 */

#ifndef SRC_C_API_REFERENCEDCVALUE_H_
#define SRC_C_API_REFERENCEDCVALUE_H_

#include "typedcvalue.h"

namespace rhdl {

template <class VALUE_TYPE>
class ReferencedCValue
		: public TypedCValue<VALUE_TYPE, !std::is_same_v<VALUE_TYPE, std::string>>
{
	using Super = TypedCValue<VALUE_TYPE, !std::is_same_v<VALUE_TYPE, std::string>>;

public:
	using ValueType = VALUE_TYPE;

	ReferencedCValue(
			std::string name, CValueContainer &container,
			const ValueType &value)
			: Super(name, container), value_(value) {}
	ReferencedCValue(
			ReferencedCValue &&moved, CValueContainer &newContainer,
			const ValueType &value)
			: Super(std::move(moved), newContainer), value_(value) {}

	virtual ~ReferencedCValue() {}

	operator typename Super::ReturnType() const {return value_;}

protected:
	const ValueType &value() {return value_;}

private:
	const ValueType &value_;
};

} /* namespace rhdl */

#endif /* SRC_C_API_REFERENCEDCVALUE_H_ */
