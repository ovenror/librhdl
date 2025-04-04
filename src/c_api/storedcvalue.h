/*
 * storedcvalue.h
 *
 *  Created on: Apr 13, 2025
 *      Author: js
 */

#ifndef SRC_C_API_STOREDCVALUE_H_
#define SRC_C_API_STOREDCVALUE_H_

#include "typedcvalue.h"

namespace rhdl {

template <class VALUE_TYPE>
class StoredCValue
		: public TypedCValue<VALUE_TYPE, !std::is_same_v<VALUE_TYPE, std::string>> {
	using Super = TypedCValue<VALUE_TYPE, !std::is_same_v<VALUE_TYPE, std::string>>;

public:
	StoredCValue(
			std::string name, CValueContainer &container,
			VALUE_TYPE value)
			: Super(name, container), value_(std::move(value)) {}
	StoredCValue(
			StoredCValue &&moved, CValueContainer &newContainer)
			: Super(std::move(moved), newContainer), value_(std::move(moved.value_)) {}
	virtual ~StoredCValue() {}

	operator typename Super::ReturnType() const override {return value_;}

protected:
	VALUE_TYPE &value() {return value_;}
	const VALUE_TYPE &value() const {return value_;}

private:
	VALUE_TYPE value_;
};

} /* namespace rhdl */

#endif /* SRC_C_API_STOREDCVALUE_H_ */
