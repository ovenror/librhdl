#ifndef NETLISTSIM_H
#define NETLISTSIM_H

#include "exposableinternalstatesim.h"
#include "representation/netlist/netlist.h"
#include <map>

namespace rhdl {

class Entity;

namespace netlist {

class NetlistSim : public ExposableInternalStateSim
{
public:
	NetlistSim(const Netlist &netlist);

	bool get(const ISingle *iface) const override;
	void internalStep() override;

protected:
	void init(const std::vector<const ISingle *> &ifaces);
	void setInternal(const ISingle *iface) override;

private:
	std::vector<const ISingle*> getIfaces(const Netlist::Interface &nli);
	const Netlist &netlist_;
	std::map<VertexRef, bool> internal_state_;
};

}}

#endif // NETLISTSIM_H
