#include "netlistsim.h"
#include "entity/entity.h"
#include "representation/netlist/netlist.h"

namespace rhdl::netlist {

NetlistSim::NetlistSim(const Netlist &netlist) :
	netlist_(netlist), internal_state_(netlist.graph().initialStateMap())
{
	init(getIfaces(netlist.ifaceMap()));
}

bool NetlistSim::get(const ISingle *iface) const
{
	//std::cerr << "get" << std::endl;
	VertexRef v;

	try {
		v = netlist_.ifaceMap().at(iface);
	}
	catch (...) {
		assert  (0);
	}

	try {
		return internal_state_.at(v);
	}
	catch (...) {
		assert  (0);
	}
}

void NetlistSim::setInternal(const ISingle *iface)
{
	//std::cerr << "setInternal" << std::endl;
	internal_state_.at(netlist_.ifaceMap().at(iface)) = true;
}

void NetlistSim::internalStep()
{
	//std::cerr << "internalStep" << std::endl;
	netlist_.graph().simStep(internal_state_);
}

void NetlistSim::init(const std::vector<const ISingle *> &ifaces)
{
	//std::cerr << "init" << std::endl;

	Simulator::init(ifaces);

	auto viters = netlist_.graph().vertices();

	for (auto viter = viters.first; viter != viters.second; ++viter)
		internal_state_[*viter] = false;
}

std::vector<const ISingle *> NetlistSim::getIfaces(const Netlist::InterfaceMap &nli)
{
	std::vector<const ISingle*> ifaces;

	for (auto &kv : nli)
		ifaces.push_back(kv.first);

	return ifaces;
}

}
