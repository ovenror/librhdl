/*
 * ValueObject.h
 *
 *  Created on: Oct 13, 2024
 *      Author: js
 */

#ifndef SRC_C_API_VALUEOBJECT_H_
#define SRC_C_API_VALUEOBJECT_H_

#include "cobjectimpl.h"
#include "cvalue.h"

namespace rhdl {

template <class C_Struct, enum rhdl_type TYPE>
class ValueObject : public CObjectImpl<
		ValueObject<C_Struct, TYPE>, C_Struct, TYPE, false, CObject, CValue> {

	using Super = CObjectImpl<
			ValueObject<C_Struct, TYPE>, C_Struct,
			TYPE, false, CObject, CValue>;
public:
	ValueObject(std::string name);
	ValueObject(ValueObject &&);

	virtual ~ValueObject();
};

} /* namespace rhdl */

#endif /* SRC_C_API_VALUEOBJECT_H_ */
