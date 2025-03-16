/*
 * RootNamespace.cpp
 *
 *  Created on: Oct 7, 2024
 *      Author: js
 */

#include <c_api/rootnamespace.h>
#include "construction/library.h"
#include "init/init.h"

#include <memory>

namespace rhdl {

RootNamespace::RootNamespace() : Namespace("root") {
	init::root_ready();
}

RootNamespace::~RootNamespace() {}

void RootNamespace::init()
{
	auto lib = std::unique_ptr<Library>(defaultLib);
	rootNamespace.add(std::move(lib));
	auto trans = std::unique_ptr<Transformer>(transformer);
	rootNamespace.add(std::move(trans));
}

RootNamespace rootNamespace;

}
