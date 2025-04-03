/*
 * cacheddynamicvalue.h
 *
 *  Created on: Oct 25, 2024
 *      Author: js
 */

#ifndef SRC_C_API_CACHEDDYNAMICCVALUE_H_
#define SRC_C_API_CACHEDDYNAMICCVALUE_H_

#include "c_api/cvalue.h"
#include "util/cached.h"

#include <string>

namespace rhdl {

template <class Container, class ValueType = std::string>
class CachedDynamicCValue : public CValue {
public:
	CachedDynamicCValue(
			std::string name, Container &container,
			void (Container::*compute)(ValueType &result) const);
	CachedDynamicCValue(CachedDynamicCValue &&);

	virtual ~CachedDynamicCValue();

	operator const ValueType &() const override {
		return cached_();
	}

	const char *to_cstring() const override;

private:
	Cached<ValueType, Container> cached_;
};

} /* namespace rhdl */

#endif /* SRC_C_API_CACHEDDYNAMICCVALUE_H_ */
