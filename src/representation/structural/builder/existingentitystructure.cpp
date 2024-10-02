/*
 * existingentitystructure.cpp
 *
 *  Created on: Jul 27, 2021
 *      Author: ovenror
 */

#include "existingentitystructure.h"
#include "portscreator.h"
#include "construction/library.h"
#include "entity/entity.h"

namespace rhdl {
namespace structural {
namespace builder {

ExistingEntityStructure::ExistingEntityStructure(const std::string &entityName)
	: ExistingEntityStructure(defaultLib -> at(entityName))
{}

ExistingEntityStructure::ExistingEntityStructure(const Entity &entity)
	: Structure(entity.isStateless()), entity_(entity),
	  top_(PortsCreator(*this).create(entity.interface()))

{}

ExistingEntityStructure::~ExistingEntityStructure()
{
	if (valid())
		doFinalize();
}


} /* namespace builder */
} /* namespace structural */
} /* namespace rhdl */
