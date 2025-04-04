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
	CObject(rhdl_type typeId, std::string name);
	CObject(CObject &&moved);

	virtual ~CObject();

	const std::string &name() const {return name_;}
	const std::string fqn() const;
	const CObject *container() const {return container_;}

	operator const rhdl_object *const() const {return c_ptr(*this);}

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

	virtual explicit operator const CObject *() const {
		throw ConstructionException(Errorcode::E_WRONG_OBJECT_TYPE);
	}

	virtual const CObject &getRef() const {
		throw ConstructionException(Errorcode::E_WRONG_OBJECT_TYPE);
	}

	explicit operator const char*() const;

	virtual bool isValue() const {return false;}

protected:
	void setMembers();

	static void updateContainerFor(const CObject &o, const CObject &c)
	{
		o.updateContainer(c);
	}

private:
	void updateContainer(const CObject &c) const {container_ = &c;}
	virtual void setMembers_internal(const char *const *members) {};

	const std::string name_;
	mutable const CObject *container_ = nullptr;

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
