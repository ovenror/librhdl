/*
 * simplifiedstructural.cpp
 *
 *  Created on: Jun 23, 2021
 *      Author: ovenror
 */

#include <representation/structural/structure.h>
#include "interface/isingle.h"

#include "simulation/hierarchicalsim.h"

#include <memory>

namespace rhdl::structural {

Structure::Structure(
		const Entity &entity,
		std::vector<const Entity *> elements, std::vector<Connection> connections,
		const Timing *timing, const Representation *parent)
	: RepresentationBase<Structure>(entity, parent, timing),
	  elements_(std::move(elements)), connections_(connections)
{}

Structure::~Structure() {}

std::unique_ptr<Simulator> Structure::makeSimulator(bool use_behavior) const
{
	return std::make_unique<HierarchicalSim>(*this, use_behavior);
}

} /* namespace rhdl::structural */
