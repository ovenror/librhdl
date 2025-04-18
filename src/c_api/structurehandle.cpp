/*
 * structurehandle.cpp
 *
 *  Created on: May 21, 2021
 *      Author: ovenror
 */

#include "structurehandle.h"
#include "representation/structural/builder/structure.h"
#include <cassert>

namespace rhdl {

namespace sb = structural::builder;

StructureHandle::StructureHandle(Namespace &ns, const std::string name, int mode)
	: TypedCObject(name),
	  structure_(sb::makeStructure(
			  ns, name, static_cast<Structure::Mode>(mode)))
{
	auto &port = structure_ -> topPort();
	c_.content().connector = rhdl::c_ptr(context_.make(port));
}

StructureHandle::~StructureHandle() {}

Handle &StructureHandle::makeComponent(const Entity &entity)
{
	return context_.make(structure_ -> add(entity));
}

void StructureHandle::abort()
{
	structure_ -> abort();
}

void StructureHandle::finalize()
{
	structure_ -> finalize();
}

} /* namespace rhdl */
