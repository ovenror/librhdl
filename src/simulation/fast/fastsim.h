#ifndef FASTSIM_H
#define FASTSIM_H

#include "../exposableinternalstatesim.h"
#include "traversal.h"
#include "sisingle_base.h"
#include <functional>

namespace rhdl {


template<class SIFACE, class INTERNAL_STATE>
class FastSim : public ExposableInternalStateSim
{
public:
	using IMap = SISingle_Base::IMap;
	using StepFunc = std::function<void(FastSim &)>;

	FastSim(const Interface *i, StepFunc stepfunc, StepFunc procfunc = [](FastSim &){} )
		: siface_(i), stepfunc_(stepfunc), procfunc_(procfunc)
	{
		siface_.toIMap(imap_);
		init(getIFaces());
	}

	SISingle_Base *translate(const ISingle *iface) const
	{
		return imap_.at(iface);
	}

	void processInput() override {
		ExposableInternalStateSim::processInput();
		procfunc_(*this);
	}

	void internalStep() override {
		stepfunc_(*this);
	}

	template <class search_strings>
	constexpr typename SIGet<SIFACE, search_strings>::Result &iface() {return SIGet<SIFACE, search_strings>::get(siface_);}

	template <class search_strings>
	constexpr bool &state() {return iface<search_strings>().state_;}

	bool get(const ISingle *iface) const override {return translate(iface) -> state_;}
	void setInternal(const ISingle *iface) override {translate(iface) -> state_ = true;}

	const std::vector<const ISingle *> getIFaces()
	{
		std::vector<const ISingle *> result;

		for (auto kv : imap_)
			result.push_back(kv.first);

		return result;
	}

	SIFACE siface_;
	INTERNAL_STATE internal_state_;
	typename SISingle_Base::IMap imap_;
	std::function<void(FastSim &)> stepfunc_;
	std::function<void(FastSim &)> procfunc_;
};

}


#endif // FASTSIM_H
