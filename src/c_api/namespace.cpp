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

Namespace::Namespace(std::string name) : CObjectImpl(name)
{}

Namespace::~Namespace() {}


}
