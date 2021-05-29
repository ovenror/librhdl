#include "timedbehavior.h"

namespace rhdl {

std::unique_ptr<Simulator> TimedBehavior::makeSimulator(bool use_behavior) const
{
	std::ignore = use_behavior;
	return simFactory_ -> make();
}

void TimedBehavior::setBehaviorFactory(std::unique_ptr<SimFactory> bhv)
{
	simFactory_ = std::move(bhv);
}

}

