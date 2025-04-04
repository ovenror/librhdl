/*
 * typedcobject.h
 *
 *  Created on: Sep 29, 2024
 *      Author: ovenror
 */

#ifndef SRC_C_API_TYPEDCOBJECT_H_
#define SRC_C_API_TYPEDCOBJECT_H_

#include "complexcobject.h"
#include "cvalueobject.h"
#include "cstructtotypeid.h"

#include <cassert>
#include <iostream>

namespace rhdl {

template <
		class CRTP, class Typed_C_Struct,
		bool OWNING = true, bool VALUE_OBJECT = false>
class TypedCObject :
		public std::conditional<
			VALUE_OBJECT, CValueObject, ComplexCObject<OWNING>>::type
{
	static constexpr rhdl_type typeId = CStructToTypeID<Typed_C_Struct>::value;

public:
	using Super = typename std::conditional<
			VALUE_OBJECT, CValueObject, ComplexCObject<OWNING>>::type;
	using C_Struct = Typed_C_Struct;

	TypedCObject(std::string name) : Super(typeId, name), c_(*this) {}
	TypedCObject(TypedCObject &&moved)
			: Super(std::move(moved)), c_(std::move(moved.c_), *this) {}

	virtual ~TypedCObject() {}

	static CRTP &recover(const C_Struct *c) {
		TypedCObject &tco = rhdl::recover<TypedCObject>(c);
		return tco.cast();
	}

protected:
	virtual CRTP &cast() = 0;

	friend class Wrapper<TypedCObject>;
	friend class Wrapper<CRTP>;

	static constexpr unsigned long C_ID = 0x7193D0813C7 + typeId;
	Wrapper<TypedCObject> c_;
};

}

#endif /* SRC_C_API_TYPEDCOBJECT_H_ */
