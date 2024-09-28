/*
 * object.h
 *
 *  Created on: Sep 16, 2024
 *      Author: ovenror
 */

#ifndef SRC_C_API_COBJECT_H_
#define SRC_C_API_COBJECT_H_

#include <rhdl/construction/c/types.h>

#include "c_api/wrapper.h"
#include "util/dictionary.h"

namespace rhdl {

class CObject : public Dictionary<std::unique_ptr<CObject>> {
public:
	CObject(rhdl_type typeId, std::string name) : c_(*this), name_(name) {
		c_.content_.type = typeId;
		c_.content_.members = c_strings();
	}

	virtual ~CObject() {}

	const std::string &name() const {return name_;}
	virtual const char *const ls() const {return 0;}
	virtual void print() const = 0;

	operator const rhdl_object *const() const {return c_ptr(*this);}

	virtual operator const rhdl_iface_struct *const() const {return 0;}

private:
	const std::string name_;

public:
	using C_Struct = rhdl_object;

protected:
	friend class Wrapper<CObject>;
	static constexpr unsigned long C_ID = 0x0813C7;
	Wrapper<CObject> c_;
};

template <class CRTP, class Typed_C_Struct, rhdl_type TYPE_ID>
class TypedCObject : public CObject {
public:
	using C_Struct = Typed_C_Struct;

	TypedCObject(std::string name) : CObject(TYPE_ID, name), c_(*this) {}

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

#endif /* SRC_C_API_COBJECT_H_ */
