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

void Namespace::init()
{
	auto lib = std::unique_ptr<Library>(defaultLib);
	rootNamespace.add(std::move(lib));
}

Namespace rootNamespace("");

}
