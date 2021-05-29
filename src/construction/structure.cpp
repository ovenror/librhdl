/*
 * entityhandle.cpp
 *
 *  Created on: Apr 3, 2021
 *      Author: ovenror
 */

#include <rhdl/construction/structure.h>
#include "construction/newentityhandle.h"
#include "construction/existingentityhandle.h"
#include "construction/library.h"

namespace rhdl {

Structure::Structure(std::string name, Mode mode)
{
	if (mode == EXISTS)
		handle_ = std::make_unique<ExistingEntityHandle>(name);
	else
		handle_ = std::make_unique<NewEntityHandle>(name, mode == CREATE_STATELESS ? true : false);
}

Structure::~Structure() {}

EntityInterface Structure::operator [](const std::string& iname) const {
	return (*handle_)[iname];
}

void Structure::finalize() {
	handle_ -> finalize();
}

const Interfacible& Structure::interfacible() const {
	return *handle_;
}

}

