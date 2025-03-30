/*
 * cachednostructcvalue.h
 *
 *  Created on: Oct 25, 2024
 *      Author: js
 */

#ifndef SRC_C_API_CACHEDNOSTRUCTCVALUE_H_
#define SRC_C_API_CACHEDNOSTRUCTCVALUE_H_

#include "c_api/cvalue.h"
#include "util/cached.h"

#include <string>

namespace rhdl {

template <class Container, class ValueType>
class CachedNoStructCValue : public CValue {
public:
	CachedNoStructCValue(
			std::string name, Container &container,
			void (Container::*compute)(ValueType &result) const);
	virtual ~CachedNoStructCValue();

	operator const ValueType &() const override {
		return (const ValueType &) cached_;
	}

	const char *to_cstring() const override;

private:
	std::vector<const char *> c_strings_;
	Cached<ValueType, Container> cached_;
};

} /* namespace rhdl */

#endif /* SRC_C_API_CACHEDNOSTRUCTCVALUE_H_ */
