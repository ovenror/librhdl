#include "representation.h"
#include "representations.h"
#include "representationtype.h"

#include "entity/entity.h"
#include "entity/timing.h"

#include "c_api/typedcvalue.h"

#include "simulation/simulator.h"

#include <cassert>

extern "C" {
#include <rhdl/construction/c/functions.h>
}

namespace rhdl {

Representation::Representation(
		const Entity &entity, TypeID id, const Representation *parent,
		const Timing *timing)
	:
	  Super(canonicalName(entity, id, parent)),
	  typeID_(id), entity_(entity), timing_(timing),
	  sibling_index_(parent ? parent -> num_descendants_ : 0),
	  reptype_("type", *this, c_.content().type),
	  parent_("parent", *this, parent),
	  content_("content", *this, &Representation::compute_content)
{
	assert (!timing || &timing -> entity() == &entity);

	c_.content().type = (rhdl_reptype) id;
	cparent() = rhdl::c_ptr(parent);
}

Representation::Representation(Representation &&moved)
	: Super(std::move(moved)), typeID_(moved.typeID_),
	  entity_(moved.entity_), timing_(moved.timing_),
	  sibling_index_(moved.sibling_index_),
	  reptype_(std::move(moved.reptype_), *this, c_.content().type),
	  parent_(std::move(moved.parent_), *this),
	  content_(std::move(moved.content_), *this)
{
	c_.content().type = (rhdl_reptype) typeID_;
}

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

std::string Representation::canonicalName(
		const Entity &entity, TypeID type,
		const Representation *parent) const
{
	std::stringstream name;
	auto sibling_index = 0;

	if (parent) {
		name << parent -> name();
		sibling_index = parent -> register_descendant();

	}
	else {
		name << entity.name();
		sibling_index = entity.representations().size();
	}

	const auto &objs = representations.objects();
	name << "_" << objs[type].name() << "_" << sibling_index;

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

void Representation::compute_content(std::string &result) const
{
	/* TODO: Replace this with pure virtual. Representations should create
	 * their own textual representation.
	 */
	result = name();
}

}
