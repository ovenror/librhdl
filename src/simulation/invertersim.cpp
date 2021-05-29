#include "invertersim.h"
#include "entity/einverter.h"

namespace rhdl {

InverterSim::InverterSim(const EInverter &inverter) : state_(false)
{
	in_ = (const ISingle *) (inverter.interface()["in"]);
	out_ = (const ISingle *) (inverter.interface()["out"]);

	init(std::vector<const ISingle *>({in_, out_}));

	state_ = false;
	inputstate_ = false;
}

bool InverterSim::get(const ISingle *iface) const
{
	if (iface == out_)
		return state_;

	if (iface == in_)
		return inputstate_;

	assert (0);
}

void InverterSim::processInput()
{
	inputstate_ = getInput(in_);
	state_ = state_ || getInput(out_);
}

void InverterSim::internalStep()
{
	assert (input().size() <= 2);
	state_ = !inputstate_;
	inputstate_ = false;
}

}

