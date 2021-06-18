#ifndef STATELESSSIM_H
#define STATELESSSIM_H

#include "fastsim.h"

namespace rhdl::behavioral {

template <class SIFACE, class INTERNAL_STATE>
class HistorySim : public FastSim<SIFACE, INTERNAL_STATE>
{
public:
	using Super = FastSim<SIFACE, INTERNAL_STATE>;
	using StepFunc = typename Super::StepFunc;

	HistorySim(const Interface *i, StepFunc stepfunc, StepFunc procfunc = [](Super &){})
		: Super(i, stepfunc, procfunc)
	{}

	void init_history(const char *iface_name, const std::vector<bool> &values)
	{
		auto siface = Super::translate(dynamic_cast<const ISingle *>((*Super::siface_.iface_)[iface_name]));

		siface -> init_history(values);
	}

	void internalStep() override {
		Super::siface_.record();
		Super::internalStep();
	}
};

}

#endif // STATELESSSIM_H
