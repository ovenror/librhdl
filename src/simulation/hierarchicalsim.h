#ifndef HIERARCHICALSIM_H
#define HIERARCHICALSIM_H

#include "simulator.h"

#include <vector>
#include <map>
#include <memory>

namespace rhdl::structural {

class CompressedStructure;
class Connection;
class Port;

class HierarchicalSim : public Simulator
{
public:
	HierarchicalSim(const Structure &structure, bool use_behavior);

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
	bool propagateConnection(const Connection &c);
	bool propagateConnection(const Port &from, const Port &to);
	bool getPortOutput(const Port &p) const;
	bool getPortInput(const Port &p) const;
	void setPort(const Port &p);

	std::vector<const ISingle *> getIFaces(const Entity &entity) const;

	const Structure &structure_;
	std::vector <std::unique_ptr <Simulator> > sub_sims_;
	std::vector <Simulator *> part_sims_;
	StateMap stateCache_;
	bool stepping_;
};

}

#endif // HIERARCHICALSIM_H
