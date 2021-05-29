#include "simulator.h"
#include "entity/entity.h"
#include "interface/visitors/flattenvisitor.h"

#include <vector>
#include <algorithm>

namespace rhdl {

Simulator::Simulator()
{
}

Simulator::~Simulator()
{
}

void Simulator::init(const std::vector<const ISingle *> &ifaces)
{
	debug_ifaces_ = ifaces;

	for (auto *iface : ifaces)
		input_[iface] = false;
}

void Simulator::set(const ISingle *iface, bool value)
{
	try {
		input_.at(iface) = value;
	}
	catch (...) {
		assert (0);
	}
}

void Simulator::resetInput()
{
	for (auto &kv : input_) {
		kv.second = false;
	}
}

void Simulator::step()
{
	processInput();
	internalStep();
}

void Simulator::processInput()
{
}

void Simulator::toStream(std::ostream &os) const
{
	os << "Simulator " << typeid(*this).name() << std::endl;
	for (const ISingle *iface : debug_ifaces_) {
		os << "  " << iface -> name() << " (" << iface << "): " << get(iface) << " (" << getInput(iface) << ")" << std::endl;
	}
}

std::ostream &operator<<(std::ostream &os, const Simulator &sim)
{
	sim.toStream(os);
	return os;
}

}
