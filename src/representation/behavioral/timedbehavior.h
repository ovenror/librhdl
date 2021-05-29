#ifndef TIMEDBEHAVIOR_H
#define TIMEDBEHAVIOR_H

#include "simulation/fast/fastsim.h"
#include "simulation/fast/fastsimfactory.h"
#include "representation/representationbase.h"

namespace rhdl {

class Timing;

class TimedBehavior : public RepresentationBase<TimedBehavior>
{
public:
	template <class siface_class, class internal_state_class>
	using SimFunc = std::function<void(FastSim<siface_class, internal_state_class> &)>;

	using SimInit = std::initializer_list<std::map<const char *, std::vector<bool> >::value_type>;

	template<class siface_class, class internal_state_class>
	TimedBehavior(
			const Entity &entity, const Timing *timing,
			SimFunc<siface_class, internal_state_class> simfunc,
			SimFunc<siface_class, internal_state_class> procfunc
				= [](FastSim<siface_class, internal_state_class> &){},
			SimInit initial = {}
			)
		: RepresentationBase<TimedBehavior>(entity, nullptr, timing)
	{
		setBehaviorFactory(std::make_unique<FastSimFactory<siface_class, internal_state_class> >(this -> entity(), simfunc, procfunc, initial));
	}

	TimedBehavior(const TimedBehavior &&) = delete;
	TimedBehavior(TimedBehavior &&) = default;

	virtual std::unique_ptr<Simulator> makeSimulator(bool use_behavior) const override;

private:
	void setBehaviorFactory(std::unique_ptr<SimFactory> bhv);

	std::unique_ptr<SimFactory> simFactory_;
};

}

#endif // TIMEDBEHAVIOR_H
