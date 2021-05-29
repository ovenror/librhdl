#ifndef HIERARCHICALSIM_H
#define HIERARCHICALSIM_H

#include "simulator.h"
#include "representation/structural/structural.h"
#include <vector>
#include <map>
#include <memory>

namespace rhdl {

class HierarchicalSim : public Simulator
{
public:
	typedef Structural::FlatConnection FlatConnection;
	typedef Structural::FlatPort FlatPort;

	HierarchicalSim(const Structural &structure, bool use_behavior);

	bool get(const ISingle *iface) const override;
	void internalStep() override;
	void processInput() override;

private:
	void stepSubSims();
	bool copyInput();
	void reset();
	void propagateDeep();

	void clearStateCache();
	void resetSubSimsInputs();

	bool propagateConnections();
	bool propagateConnectionsOnce();
	bool propagateConnection(const FlatConnection &c);
	bool propagateConnection(const FlatPort &from, const FlatPort &to);
	bool getPortOutput(const FlatPort &p) const;
	bool getPortInput(const FlatPort &p) const;
	void setPort(const FlatPort &p);

	std::vector<const ISingle *> getIFaces(const Entity &entity) const;

	const Structural &structure_;
	std::vector <std::unique_ptr <Simulator> > sub_sims_;
	std::vector <Simulator *> part_sims_;
	StateMap stateCache_;
	bool stepping_;
};

}

#endif // HIERARCHICALSIM_H
