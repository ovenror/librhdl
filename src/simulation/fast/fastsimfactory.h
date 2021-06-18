#ifndef FASTSIMFACTORY_H
#define FASTSIMFACTORY_H

#include "fastsim.h"
#include "historysim.h"
#include "simulation/simfactory.h"
#include <memory>
#include <functional>

namespace rhdl {

class Entity;
class NO_STATE {};

namespace behavioral {

template <class SIFACE, class INTERNAL_STATE>
class FastSimFactory : public SimFactory
{
public:
	using Sim = FastSim<SIFACE, INTERNAL_STATE>;
	using HSim = HistorySim<SIFACE, INTERNAL_STATE>;
	using StepFunc = std::function<void(Sim &)>;
	using InitMap = std::map<const char *, std::vector<bool> >;
	using Fused = std::vector<std::vector<const char *>>;
	using Initial = std::initializer_list<InitMap::value_type>;

	FastSimFactory(StepFunc f, StepFunc p = [](Sim &){}, Initial initial={}) :
		stepfunc_(f), procfunc_(p), initmap_(initial)
	{}

	std::unique_ptr<Simulator> make(const Interface &iface) override
	{
		if (!initmap_.size())
			return std::make_unique<Sim>(&iface, stepfunc_, procfunc_);

		auto sim = std::make_unique<HSim>(&iface, stepfunc_, procfunc_);

		for (auto kv : initmap_)
		{
			sim -> init_history(kv.first, kv.second);
		}

		return sim;
	}

	StepFunc stepfunc_;
	StepFunc procfunc_;
	InitMap initmap_;
};

} /* namespace behavioral */
} /* namespace rhdl */


#endif // FASTSIMFACTORY_H
