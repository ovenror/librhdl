/*
 * namespace.h
 *
 *  Created on: Sep 28, 2024
 *      Author: ovenror
 */

#ifndef SRC_C_API_NAMESPACE_H_
#define SRC_C_API_NAMESPACE_H_

#include "typedcomplexcobject.h"
#include <cassert>

namespace rhdl {

class Namespace : public TypedComplexCObject<Namespace, rhdl_namespace_struct> {
	using Super = TypedComplexCObject<Namespace, rhdl_namespace_struct>;

public:
	Namespace(std::string);
	virtual ~Namespace();

	Namespace &cast() override {return *this;};

private:
	void setTypedMembers(const char *const *) override;
};

} /* namespace rhdl */

#endif /* SRC_C_API_NAMESPACE_H_ */
