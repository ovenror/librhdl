/*
 * cvalue.h
 *
 *  Created on: Oct 3, 2024
 *      Author: ovenror
 */

#ifndef SRC_C_API_CVALUE_H_
#define SRC_C_API_CVALUE_H_

#include "cobject.h"

namespace rhdl {

class CValue : public CObject {
public:
	CValue(rhdl_type typeId, std::string name);
	virtual ~CValue();

	explicit operator const char*() const override {return to_cstring();}

	explicit operator int64_t() const override {
		throw ConstructionException(Errorcode::E_WRONG_VALUE_TYPE);
	}

	explicit operator uint64_t() const override {
		throw ConstructionException(Errorcode::E_WRONG_VALUE_TYPE);
	}

	explicit operator rhdl_direction() const override {
		throw ConstructionException(Errorcode::E_WRONG_VALUE_TYPE);
	}

	bool isValue() const override {return true;}

private:
	virtual const char *to_cstring() const = 0;
};

} /* namespace rhdl */

#endif /* SRC_C_API_CVALUE_H_ */
