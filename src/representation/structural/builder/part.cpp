/*
 * pert.cpp
 *
 *  Created on: Jun 20, 2021
 *      Author: ovenror
 */

#include "part.h"
#include "portscreator.h"
#include "partialstructurebuilder.h"

#include "construction/library.h"

#include "entity/entity.h"

namespace rhdl::structural::builder {

Part::Part(const Entity &entity)
	: entity_(entity),
	  top_(PortsCreator(*this).create(entity.interface()))
{}

Part::~Part() {
	// TODO Auto-generated destructor stub
}

void Part::toStream(std::ostream &os) const
{
	os << "(" << entity_.name() << " instance)@" << this;
}

ComplexPort& makeComponent(const std::string &entityName)
{
	return (new PartialStructureBuilder()) -> add(defaultLib -> at(entityName));
}

} /* namespace rhdl::structural::builder */
