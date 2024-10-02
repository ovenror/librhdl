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

	static void init();

	Namespace &cast() override {return *this;};

private:
	void setMembers_internal(const std::vector<const char *> &members) override {
		c_.content().members = members.data();
	}
};

extern Namespace rootNamespace;

} /* namespace rhdl */

#endif /* SRC_C_API_NAMESPACE_H_ */
