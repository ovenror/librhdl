#include "representation.h"
#include "representations.h"

#include "entity/entity.h"
#include "entity/timing.h"

#include "simulation/simulator.h"

#include <cassert>

namespace rhdl {

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

void Representation::breakTiming()
{
	timing_ = &entity_.addTiming();
}

Representation::Representation(
		const Entity &entity, TypeID id, const Representation *parent,
		const Timing *timing)
	:
	  typeID_(id), entity_(entity), parent_(parent), timing_(timing)
{
	assert (!timing || &timing -> entity() == &entity);
}

Representation::Representation(const Entity &entity)
	: Representation(entity, Representations::INVALID, nullptr)
{}

std::unique_ptr<Simulator> Representation::makeSimulator(
		bool use_behavior) const
{
	return nullptr;
}

}
