/*
 * namespace.h
 *
 *  Created on: Sep 28, 2024
 *      Author: ovenror
 */

#ifndef SRC_C_API_NAMESPACE_H_
#define SRC_C_API_NAMESPACE_H_

#include "typedcomplexcobject.h"
#include "../util/lexicaldictionary.h"
#include <cassert>

namespace rhdl {

class Namespace : public TypedComplexCObject<Namespace, rhdl_namespace_struct> {
public:
	Namespace(std::string);
	virtual ~Namespace();

	Namespace &cast() override {return *this;};

private:
	virtual operator Namespace &() override {return *this;}
	virtual operator const Namespace &() const override {return *this;}

	void setMembers() override;
	void setTypedMembers();
};

} /* namespace rhdl */

#endif /* SRC_C_API_NAMESPACE_H_ */
