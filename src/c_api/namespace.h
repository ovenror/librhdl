/*
 * namespace.h
 *
 *  Created on: Sep 28, 2024
 *      Author: ovenror
 */

#ifndef SRC_C_API_NAMESPACE_H_
#define SRC_C_API_NAMESPACE_H_

#include "../util/lexicaldictionary.h"
#include "cobjectimpl.h"

#include <cassert>

namespace rhdl {

class Namespace : public CObjectImpl<Namespace, rhdl_namespace_struct, RHDL_NAMESPACE> {
public:
	Namespace(std::string);
	virtual ~Namespace();

	Namespace &cast() override {return *this;};

private:
	void setMembers_internal(const char *const *members) override {
		c_.content().members = members;
	}
};

} /* namespace rhdl */

#endif /* SRC_C_API_NAMESPACE_H_ */
