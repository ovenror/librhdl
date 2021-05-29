/*
 * structurehandle.cpp
 *
 *  Created on: May 21, 2021
 *      Author: ovenror
 */

#include "structurehandle.h"

namespace rhdl {

StructureHandle::StructureHandle(const std::string name, int mode) : c_(*this)
{
	connector_ = &context_.make<Structure>(name, static_cast<Structure::Mode>(mode));
	c_.content().connector = c_ptr(*static_cast<Handle *>(connector_));
}

Handle &StructureHandle::makeComponent(const Entity &entity)
{
	return context_.make<PartHandle>(&entity);
}

void StructureHandle::finalize() {
	try {
		connector_ -> finalize();
	}
	catch (ConstructionException &e) {
		assert (e.errorcode() == Errorcode::E_NETLIST_CONTAINS_CYCLES);
		throw;
	}
}

} /* namespace rhdl */
