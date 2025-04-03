/*
 * typedcobject.h
 *
 *  Created on: Sep 29, 2024
 *      Author: ovenror
 */

#ifndef SRC_C_API_TYPEDCOBJECT_H_
#define SRC_C_API_TYPEDCOBJECT_H_

#include "cobject.h"

#include <cassert>
#include <iostream>

namespace rhdl {

template <class CRTP, class Typed_C_Struct, rhdl_type TYPE_ID, class BASE = CObject>
class TypedCObject : public BASE {
public:
	using C_Struct = Typed_C_Struct;

	TypedCObject(std::string name) : BASE(TYPE_ID, name), c_(*this) {}
	TypedCObject(TypedCObject &&moved) : BASE(TYPE_ID, moved.name()), c_(*this) {}

	virtual ~TypedCObject() {}

	static CRTP &recover(const C_Struct *c) {
		TypedCObject &tco = rhdl::recover<TypedCObject>(c);
		return tco.cast();
	}

protected:
	virtual CRTP &cast() = 0;

	friend class Wrapper<TypedCObject>;
	friend class Wrapper<CRTP>;

	static constexpr unsigned long C_ID = 0x7193D0813C7 + TYPE_ID;
	Wrapper<TypedCObject> c_;
};

}

#endif /* SRC_C_API_TYPEDCOBJECT_H_ */
