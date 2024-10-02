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

template <class CRTP, class Typed_C_Struct, rhdl_type TYPE_ID>
class TypedCObject : public CObject {
public:
	using C_Struct = Typed_C_Struct;

	TypedCObject(std::string name) : CObject(TYPE_ID, name), c_(*this) {}
	virtual ~TypedCObject() {}

	static CRTP &recover(const C_Struct *c) {
		TypedCObject &tco = rhdl::recover<TypedCObject>(c);
		return tco.cast();
	}

	void setMembers(const std::vector<const char *> &members) {
		auto lol = members.data();
		setMembers_internal(members);
		CObject::setMembers(members);
	}

private:
	virtual void setMembers_internal(const std::vector<const char *> &members) {};

protected:
	virtual CRTP &cast() = 0;

	friend class Wrapper<TypedCObject>;
	friend class Wrapper<CRTP>;

	static constexpr unsigned long C_ID = 0x7193D0813C7 + TYPE_ID;
	Wrapper<TypedCObject> c_;
};

}

#endif /* SRC_C_API_TYPEDCOBJECT_H_ */
