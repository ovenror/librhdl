/*
 * valueobject.h
 *
 *  Created on: Apr 5, 2025
 *      Author: js
 */

#ifndef SRC_C_API_CVALUEOBJECT_H_
#define SRC_C_API_CVALUEOBJECT_H_

#include <c_api/complexcobject.h>

namespace rhdl {

class CValueObject : public ComplexCObject<false> {
	using Super = ComplexCObject<false>;

public:
	CValueObject(rhdl_type typeId, std::string name);
	CValueObject(CValueObject &&moved);
	virtual ~CValueObject();

	const CObject &add_after_move(const CValue &cvalue) override;
};

} /* namespace rhdl */

#endif /* SRC_C_API_CVALUEOBJECT_H_ */
