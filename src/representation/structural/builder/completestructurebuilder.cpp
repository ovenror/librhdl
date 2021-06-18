/*
 * completestructurebuilder.cpp
 *
 *  Created on: Aug 11, 2021
 *      Author: ovenror
 */

#include "completestructurebuilder.h"
#include "partialstructurebuilder.h"
#include "structure.h"

#include "../structure.h"
#include "../element.h"

#include "entity/entity.h"

namespace rhdl {
namespace structural {
namespace builder {

CompleteStructureBuilder::CompleteStructureBuilder(bool stateless)
	: StructureBuilder(stateless)
{}

CompleteStructureBuilder::~CompleteStructureBuilder() {}

void CompleteStructureBuilder::combineWith(StructureBuilder &b)
{
	b.combineWith(*this);
}

void CompleteStructureBuilder::combineWith(CompleteStructureBuilder &s)
{
	assert (0);
}

structural::Structure CompleteStructureBuilder::construct(
		Structure &structure, const Entity &entity) const
{
	std::vector<const Entity *> elements{&entity};

	auto elementMap = buildParts(elements);
	elementMap[&structure] = structureIdx;

	auto connections = buildConnections(elementMap);

	return structural::Structure(
			entity, std::move(elements), std::move(connections),
			&entity.addTiming(), nullptr);
}

void CompleteStructureBuilder::setStateful()
{
	throw ConstructionException(Errorcode::E_STATEFUL_COMPONENT_IN_STATELESS_ENTITY);
}

} /* namespace builder */
} /* namespace structural */
} /* namespace rhdl */
