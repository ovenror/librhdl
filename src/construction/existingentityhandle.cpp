/*
 * existingentityhandle.cpp
 *
 *  Created on: May 15, 2021
 *      Author: ovenror
 */

#include "construction/existingentityhandle.h"
#include "construction/library.h"

namespace rhdl {

ExistingEntityHandle::ExistingEntityHandle(const std::string& name)
	: ExistingEntityHandle(*defaultLib.entities().at(name))
{}

ExistingEntityHandle::ExistingEntityHandle(const Entity& entity)
	: EntityHandle(entity), entity_(entity)
{}

ExistingEntityHandle::~ExistingEntityHandle() {
	finalize();
}

void ExistingEntityHandle::finalize() {
	finalizeOnce();
}

} /* namespace rhdl */
