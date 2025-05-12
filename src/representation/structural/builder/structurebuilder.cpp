#include "structurebuilder.h"
#include "partialstructurebuilder.h"
#include "connection.h"
#include "part.h"
#include "existingport.h"
#include "connectionsbuilder.h"

#include "entity/entity.h"

#include "interface/icomposite.h"

#include <rhdl/construction/constructionexception.h>
#include <rhdl/construction/connectible.h>

#include <cassert>
#include <queue>
#include <set>

namespace rhdl::structural::builder {

StructureBuilder::StructureBuilder(bool stateless)
	: parts_(*this), stateless_(stateless)
{}

StructureBuilder::~StructureBuilder() {}

std::map<Element *, ElementIdx> StructureBuilder::buildParts(
		std::vector<const Entity *> &target) const
{
	std::map<Element *, ElementIdx> result;

	for (auto &part : parts_) {
		result[&part] = target.size();
		target.emplace_back(&part.entity());
	}

	return result;
}

std::vector<structural::Connection>
	StructureBuilder::buildConnections(
		const std::map<Element *, ElementIdx> &elementMap) const
{
	return ConnectionsBuilder(elementMap).build();
}

void StructureBuilder::ingest(PartialStructureBuilder &victim)
{
	assert (this != &victim);

	if (stateless_ && !victim.stateless())
		setStateful();

	parts_.ingest(victim.parts_);
}

void StructureBuilder::release(PartitionClassBase<Part> &)
{
	delete this;
}

void StructureBuilder::combineIffNotSame(
		StructureBuilder &b1, StructureBuilder &b2)
{
	if (&b1 != &b2)
		b1.combineWith(b2);
}

ComplexPort &StructureBuilder::add(const Entity &partEntity, const std::string *name)
{
	if (stateless_ && !partEntity.isStateless())
		setStateful();

	return parts_.accept(std::make_unique<Part>(partEntity, name)).topPort();
}

void StructureBuilder::invalidate()
{
	for (auto &part : parts())
		part.invalidate();
}

void StructureBuilder::assertStatelessness(const Part &p)
{
	assert (!stateless_ || p.entity().isStateless());
}

}
