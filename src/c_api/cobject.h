/*
 * object.h
 *
 *  Created on: Sep 16, 2024
 *      Author: ovenror
 */

#ifndef SRC_C_API_COBJECT_H_
#define SRC_C_API_COBJECT_H_

#include <rhdl/construction/c/types.h>
#include "../util/dictionary.h"
#include "c_api/wrapper.h"

namespace rhdl {

class CObject : public Dictionary<CObject> {
public:
	CObject(rhdl_type typeId, std::string name) : c_(*this), name_(name) {
		c_.content_.type = typeId;
	}

	virtual ~CObject() {}

	const std::string &name() const {return name_;}

	operator const rhdl_object *const() const {return c_ptr(*this);}

	virtual operator const rhdl_iface_struct *const() const {return 0;}

protected:
	void setMembers(const std::vector<const char *> &members) {
		c_.content_.members = members.data();
	}

private:
	const std::string name_;

public:
	using C_Struct = rhdl_object;

private:
	friend class Wrapper<CObject>;
	static constexpr unsigned long C_ID = 0x0813C7;
	Wrapper<CObject> c_;
};

}

#endif /* SRC_C_API_COBJECT_H_ */
