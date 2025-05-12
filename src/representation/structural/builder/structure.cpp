/*
 * structure.cpp
 *
 *  Created on: Jun 20, 2021
 *      Author: ovenror
 */

#include "structure.h"
#include "existingentitystructure.h"
#include "newentitystructure.h"
#include "partialstructurebuilder.h"
#include "builderport.h"

#include "../structure.h"

#include "representation/behavioral/timedbehavior.h"
#include "representation/behavioral/functionalbehavior.h"

#include <rhdl/construction/structure.h>

namespace rhdl::structural::builder {

using Mode = rhdl::Structure::Mode;

using behavioral::TimedBehavior;

Structure::Structure(bool stateless) : builder_(stateless)
{}

Structure::~Structure() {}

ComplexPort &Structure::add(const Entity &partEntity, const std::string *name)
{
	return builder_.add(partEntity, name);
}

void Structure::toStream(std::ostream &os) const
{
	os << "(new Structure for " << name() << ")@" << this;
}

void Structure::finalize()
{
	expect_valid();
	doFinalize();
}

void Structure::doFinalize()
{
	const Entity &e = entity();

	try {
		auto &structure = e.addRepresentation(builder_.construct(*this, e));

		invalidate();

		for (auto &behavior : behavior_) {
			const Timing *timing = behavior.second ? structure.timing() : nullptr;
			auto behav = TimedBehavior(e, timing, std::move(behavior.first));
			e.addRepresentation(std::move(behav));
		}
	}
	catch (const ConstructionException &e) {
		assert (e.errorcode() == Errorcode::E_NETLIST_CONTAINS_CYCLES);
		abort();
		throw e;
	}
}

void Structure::abort()
{
	expect_valid();
	invalidate();
}

void Structure::invalidate()
{
	valid_ = false;
	Element::invalidate();
	builder_.invalidate();
}

std::unique_ptr<Structure> makeStructure(
		Namespace &ns, const std::string &entityName, Mode mode)
{
	if (mode == Mode::EXISTS) {
		return std::make_unique<ExistingEntityStructure>(ns, entityName);
	}
	else {
		assert (mode == Mode::CREATE_STATEFUL || mode == Mode::CREATE_STATELESS);
		return std::make_unique<NewEntityStructure>(ns, entityName, mode == Mode::CREATE_STATELESS);
	}
}

void Structure::expect_valid()
{
	if (!valid_)
		throw ConstructionException(Errorcode::E_INVALID_HANDLE);
}

} /* namespace rhdl::structural::builder */
