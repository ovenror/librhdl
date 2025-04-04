/*
 * cacheddynamicvalue.h
 *
 *  Created on: Oct 25, 2024
 *      Author: js
 */

#ifndef SRC_C_API_CACHEDDYNAMICCVALUE_H_
#define SRC_C_API_CACHEDDYNAMICCVALUE_H_

#include <c_api/referencedcvalue.h>
#include "util/cached.h"

#include <string>

namespace rhdl {

template <class CONTAINER, class VALUE_TYPE>
class CachedDynamicCValue : public TypedCValue<
		VALUE_TYPE, !std::is_same<VALUE_TYPE, std::string>()>
{
	using Super = TypedCValue<
			VALUE_TYPE, !std::is_same<VALUE_TYPE, std::string>()>;

public:
	CachedDynamicCValue(
			std::string name, CONTAINER &container,
			void (CONTAINER::*compute)(VALUE_TYPE &result) const)
			: Super(name, container), cached_(container, compute) {}

	CachedDynamicCValue(CachedDynamicCValue &&moved, CONTAINER &newContainer)
			: Super(std::move(moved), newContainer),
			  cached_(std::move(moved.cached_), newContainer) {}

	virtual ~CachedDynamicCValue() {}

	operator typename Super::ReturnType() const override {return value();}

protected:
	const VALUE_TYPE &value() const {return cached_();}

private:
	Cached<VALUE_TYPE, CONTAINER> cached_;
};

} /* namespace rhdl */

#endif /* SRC_C_API_CACHEDDYNAMICCVALUE_H_ */
