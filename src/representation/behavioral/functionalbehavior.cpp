#include "functionalbehavior.h"
#include "simulation/simulator.h"

namespace rhdl {

FunctionalBehavior::FunctionalBehavior(const Entity &entity, const Representation *parent)
	: RepresentationBase<FunctionalBehavior>(entity, parent, nullptr)
{

}

std::unique_ptr<Simulator> FunctionalBehavior::makeSimulator(bool use_behavior) const
{
	std::ignore = use_behavior;
	return nullptr;
}

}
