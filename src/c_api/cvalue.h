/*
 * cvalue.h
 *
 *  Created on: Oct 3, 2024
 *      Author: ovenror
 */

#ifndef SRC_C_API_CVALUE_H_
#define SRC_C_API_CVALUE_H_

#include "cobject.h"
#include <memory>

namespace rhdl {

template <bool> class ComplexCObject;

class CValueContainer;

class CValue : public CObject {
public:
	CValue(rhdl_type typeId, std::string name, CValueContainer &container);
	CValue(CValue &&moved, CValueContainer &newContainer);

	virtual ~CValue();

	explicit operator int64_t() const override {
		throw ConstructionException(Errorcode::E_WRONG_VALUE_TYPE);
	}

	explicit operator uint64_t() const override {
		throw ConstructionException(Errorcode::E_WRONG_VALUE_TYPE);
	}

	explicit operator rhdl_direction() const override {
		throw ConstructionException(Errorcode::E_WRONG_VALUE_TYPE);
	}

	explicit operator rhdl_reptype() const override {
		throw ConstructionException(Errorcode::E_WRONG_VALUE_TYPE);
	}

	explicit operator const std::string &() const override {
		throw ConstructionException(Errorcode::E_WRONG_VALUE_TYPE);
	}

	explicit operator const CObject *() const override {
		throw ConstructionException(Errorcode::E_WRONG_VALUE_TYPE);
	}

	const CObject &getRef() const override {
		throw ConstructionException(Errorcode::E_WRONG_VALUE_TYPE);
	}

	bool isValue() const override {return true;}
};

} /* namespace rhdl */

#endif /* SRC_C_API_CVALUE_H_ */
