#include "representation.h"
#include "representations.h"

#include "entity/entity.h"
#include "entity/timing.h"

#include "simulation/simulator.h"

#include <cassert>

namespace rhdl {

Representation::~Representation() {
}

bool Representation::isChild(const Representation *r) const
{
	assert (r);

	return parent_ == r;
}

bool Representation::isDescendant(const Representation *r) const
{
	assert (r);

	if (!parent_)
		return false;

	if (isChild(r))
		return true;

	return parent_->isDescendant(r);
}

/* may not be called, if *this is already added to the entity
 * FIXME: Use lookup data structure and add assertion
 * ... However, the entity only holds const references to representations,
 * so that should not happen. */
void Representation::breakTiming()
{
	timing_ = &entity_.addTiming();
}

Representation::Representation(
		const Entity &entity, TypeID id, const Representation *parent,
		const Timing *timing)
	:
	  typeID_(id), entity_(entity), parent_(parent), timing_(timing),
	  sibling_index_(parent_ ? parent_ -> register_descendant() : 0)
{
	assert (!timing || &timing -> entity() == &entity);

	LOG(LOG_REP) << "Creating representation " << canonicalName() << std::endl;
}

std::string Representation::canonicalName() const
{
	std::stringstream name;

	if (parent_)
		name << parent_ -> canonicalName();
	else
		name << entity_.name();

	name << "_" << typeID() << sibling_index_;

	return name.str();
}

Representation::Representation(const Entity &entity)
	: Representation(entity, Representations::INVALID, nullptr)
{}

std::unique_ptr<Simulator> Representation::makeSimulator(
		bool use_behavior) const
{
	return nullptr;
}

size_t Representation::register_descendant() const
{
	return num_descendants_++;
}

}
