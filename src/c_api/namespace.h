/*
 * namespace.h
 *
 *  Created on: Sep 28, 2024
 *      Author: ovenror
 */

#ifndef SRC_C_API_NAMESPACE_H_
#define SRC_C_API_NAMESPACE_H_

#include <c_api/typedcobject.h>
#include "../util/lexicaldictionary.h"
#include <cassert>

namespace rhdl {

class Namespace : public TypedCObject<Namespace, rhdl_namespace_struct> {
public:
	Namespace(std::string);
	virtual ~Namespace();

	Namespace &cast() override {return *this;};

private:
	virtual operator Namespace &() override {return *this;}
	virtual operator const Namespace &() const override {return *this;}

	void setMembers_internal(const char *const *members) override {
		c_.content().members = members;
	}
};

} /* namespace rhdl */

#endif /* SRC_C_API_NAMESPACE_H_ */
