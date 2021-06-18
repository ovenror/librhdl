#include "timedbehavior.h"
#include "entity/entity.h"

namespace rhdl::behavioral {

std::unique_ptr<Simulator> TimedBehavior::makeSimulator(bool use_behavior) const
{
	std::ignore = use_behavior;
	return simFactory_ -> make(entity().interface());
}

TimedBehavior::TimedBehavior(
		const Entity &entity, const Timing *timing,
		std::unique_ptr<SimFactory> &&simFactory)
	: RepresentationBase<TimedBehavior>(entity, nullptr, timing),
	  simFactory_(std::move(simFactory))
{}

TimedBehavior::~TimedBehavior() {}

}

