/*
 * namespace.cc
 *
 *  Created on: Sep 28, 2024
 *      Author: ovenror
 */

#include "namespace.h"

#include "construction/library.h"

#include <memory>
#include <iostream>

namespace rhdl {

Namespace::Namespace(std::string name) : TypedCObject(name)
{
	setTypedMembers();
}

Namespace::~Namespace() {}

void Namespace::setMembers()
{
	Super::setMembers();
	setTypedMembers();
}

void Namespace::setTypedMembers()
{
	c_ptr() -> members = c_strings().data();
}

}
