/*
 * newentityhandle.cpp
 *
 *  Created on: May 15, 2021
 *      Author: ovenror
 */

#include "construction/newentityhandle.h"
#include "construction/library.h"

namespace rhdl {

NewEntityHandle::NewEntityHandle(const std::string& name, bool stateless)
	: NewEntityHandle(std::make_unique<Entity>(name, stateless))
{}

NewEntityHandle::NewEntityHandle(std::unique_ptr<Entity>&& entity)
	: EntityHandle(*entity), entity_(std::move(entity)), placeholder_(&entity_ -> interface_)
{
	if (defaultLib.contains(entity_ -> name()))
		throw ConstructionException(Errorcode::E_ENTITY_EXISTS);
}

NewEntityHandle::~NewEntityHandle() {
	finalize();
}

void NewEntityHandle::finalize() {
	if (finalizeOnce()) {
		placeholder_.finalize();
		defaultLib.regist(std::move(entity_));
	}
}

} /* namespace rhdl */

