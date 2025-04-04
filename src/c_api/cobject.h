/*
 * object.h
 *
 *  Created on: Sep 16, 2024
 *      Author: ovenror
 */

#ifndef SRC_C_API_COBJECT_H_
#define SRC_C_API_COBJECT_H_

#include <rhdl/construction/c/types.h>
#include "util/dictionary.h"
#include "util/cached.h"
#include "c_api/wrapper.h"

#include <cassert>

namespace rhdl {

class CObject : public Dictionary<CObject> {
public:
	CObject(rhdl_type typeId, std::string name) : c_(*this), name_(name) {
		c_.content_.type = typeId;
		c_.content_.name = name_.c_str();
	}

	CObject(CObject &&moved) :
			c_(*this, std::move(moved)),
			name_(std::move(moved.name_))
	{
		assert(c_.content_.name == name_.c_str());
	}

	virtual ~CObject() {}

	const std::string &name() const {return name_;}

	operator const rhdl_object *const() const {return c_ptr(*this);}

	virtual explicit operator const char*() const {
		throw ConstructionException(Errorcode::E_WRONG_OBJECT_TYPE);
	}

	virtual explicit operator int64_t() const {
		throw ConstructionException(Errorcode::E_WRONG_OBJECT_TYPE);
	}

	virtual explicit operator uint64_t() const {
		throw ConstructionException(Errorcode::E_WRONG_OBJECT_TYPE);
	}

	virtual explicit operator rhdl_direction() const {
		throw ConstructionException(Errorcode::E_WRONG_OBJECT_TYPE);
	}

	virtual explicit operator rhdl_reptype() const {
		throw ConstructionException(Errorcode::E_WRONG_OBJECT_TYPE);
	}

	virtual explicit operator const rhdl_iface_struct *const() const {
		throw ConstructionException(Errorcode::E_WRONG_OBJECT_TYPE);
	}

	virtual explicit operator const std::string&() const {
		throw ConstructionException(Errorcode::E_WRONG_OBJECT_TYPE);
	}

	virtual bool isValue() const {return false;}

protected:
	void setMembers() {
		auto members = c_strings().data();
		c_.content_.members = members;
		setMembers_internal(members);
	}

private:
	virtual void setMembers_internal(const char *const *members) {};

	const std::string name_;

public:
	using C_Struct = rhdl_object;

private:
	friend class Wrapper<CObject>;

	static constexpr unsigned long C_ID = 0x0813C7;

public:
	Wrapper<CObject> c_;
};

}

#endif /* SRC_C_API_COBJECT_H_ */
