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

StructureHandle::StructureHandle(const std::string name, int mode)
	: structure_(sb::makeStructure(name, static_cast<Structure::Mode>(mode))), c_(*this)
{
	auto &port = structure_ -> topPort();
	c_.content().connector = c_ptr(context_.make(port));
}

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
