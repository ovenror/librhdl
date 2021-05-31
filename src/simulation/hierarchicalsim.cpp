#include "hierarchicalsim.h"
#include "representation/structural/structural.h"
#include "entity/entity.h"
#include <tuple>
#include <memory>
#include <iostream>

namespace rhdl {

HierarchicalSim::HierarchicalSim(const Structural &structure, bool use_behavior) :
	structure_(structure), stepping_(false)
{
	auto ipart = structure.parts().begin();
	++ipart; //skip ourself
	part_sims_.push_back(this);

	for (;ipart != structure.parts().end(); ++ipart) {
		const Entity &part = **ipart;
		const Representation *partRep = nullptr;

		if (use_behavior) {
			partRep = part.getRepresentation(TimedBehavior::ID);

			if (!partRep || partRep -> timing() != part.defaultTiming())
				partRep = part.getRepresentation(Structural::ID);
		}
		else {
			partRep = part.getRepresentation(Structural::ID);

			if (!partRep)
				partRep = part.getRepresentation(TimedBehavior::ID);
		}

		assert (partRep && partRep -> timing() == part.defaultTiming());

		std::unique_ptr<Simulator> sim = partRep -> makeSimulator(use_behavior);
		part_sims_.push_back(sim.get());
		sub_sims_.push_back(std::move(sim));
	}

	const auto ifaces = getIFaces(structure.entity());
	init(ifaces);
	for (const ISingle *iface : ifaces)
		stateCache_[iface] = false;
}

bool HierarchicalSim::get(const ISingle *iface) const
{
	return stateCache_.at(iface);
}

/* TODO: check if copyInput changes anything.
 * If not, there is no need to propagate anything*/
void HierarchicalSim::processInput()
{
	bool changed = copyInput();

	//std::cerr << "--------" << std::endl;
	//std::cerr << "HIER " << this << "AFTER INPUT COPY" << std::endl;
	//std::cerr << *this;
	//std::cerr << "--------" << std::endl;

	if (changed) {
		propagateConnections();

		//std::cerr << "--------" << std::endl;
		//std::cerr << "HIER " << this << "AFTER INITIAL PROPAGATE CONNECTIONS" << std::endl;
		//std::cerr << *this;
		//std::cerr << "--------" << std::endl;

		propagateDeep();
	}

	//else {
	//    std::cerr << "NO CHANGE -> NO PROPAGA" << std::endl;
	//}
}

void HierarchicalSim::internalStep()
{
	stepSubSims();
	reset();
	propagateDeep();
}

void HierarchicalSim::reset()
{
	clearStateCache();
	resetSubSimsInputs();
}

void HierarchicalSim::stepSubSims()
{
	for (auto &psim : sub_sims_) {
		//std::cerr << "hier partsim begin" << std::endl;
		//std::cerr << "hier partsim >>> before state" << std::endl;
		//std::cerr << *psim;
		//std::cerr << "hier partsim <<<" << std::endl;
		psim -> internalStep();
		//std::cerr << "hier partsim >>> after state" << std::endl;
		//std::cerr << *psim;
		//std::cerr << "hier partsim <<<" << std::endl;
		//std::cerr << "hier partsim end" << std::endl;
	}
}

bool HierarchicalSim::copyInput()
{
	bool changed = false;

	for (auto &kv : input()) {
		if (!kv.second)
			continue;

		bool &stateBit = stateCache_.at(kv.first);

		if (stateBit)
			continue;

		stateBit = true;
		changed = true;
	}

	return changed;
}


void HierarchicalSim::clearStateCache()
{
	for (auto &kv : stateCache_) {
		kv.second = false;
	}
}

void HierarchicalSim::resetSubSimsInputs()
{
	for (auto &psim : sub_sims_)
		psim -> resetInput();
}

void HierarchicalSim::propagateDeep()
{
	do {
		for (auto &psim : sub_sims_) {
			//std::cerr << "--------" << std::endl;
			//std::cerr << "HIERSUB " << psim.get() << " BEFORE PROCINP" << std::endl;
			//std::cerr << *psim;
			//std::cerr << "--------" << std::endl;
			psim -> processInput();
			//std::cerr << "--------" << std::endl;
			//std::cerr << "HIERSUB " << psim.get() << " AFTER PROCINP" << std::endl;
			//std::cerr << *psim;
			//std::cerr << "--------" << std::endl;
		}
	}
	while (propagateConnections());

	//std::cerr << "--------" << std::endl;
	//std::cerr << "HIER " << this << " AFTER PROPAGA" << std::endl;
	//std::cerr << *this;
	//std::cerr << "--------" << std::endl;
}

/* TODO: Maybe remember "changed" individually for each subsim,
 * so there is no need to propagate (or check for input changes) */
bool HierarchicalSim::propagateConnections()
{ 
	bool changed = false;

	while (propagateConnectionsOnce()) {
		changed = true;
	}

	return changed;
}

bool HierarchicalSim::propagateConnectionsOnce()
{
	bool changed = false;
	for (const FlatConnection &c : structure_.flatConnections()) {
		if (propagateConnection(c))
			changed = true;
	}
	return changed;
}

bool HierarchicalSim::propagateConnection(const FlatConnection &c)
{
	FlatPort p1, p2;
	std::tie(p1, p2) = c;

	bool changed;

	changed = propagateConnection(p1, p2);

	if (propagateConnection(p2, p1))
		changed = true;

	return changed;
}

bool HierarchicalSim::propagateConnection(const HierarchicalSim::FlatPort &from, const HierarchicalSim::FlatPort &to)
{
	if (!getPortOutput(from))
		return false;

	if (getPortInput(to))
		return false;

	setPort(to);
	return true;
}

bool HierarchicalSim::getPortInput(const HierarchicalSim::FlatPort &p) const
{
	if (Structural::isExternal(p))
		return stateCache_.at(p.second);
	else
		return part_sims_[p.first]->getInput(p.second);
}


bool HierarchicalSim::getPortOutput(const HierarchicalSim::FlatPort &p) const
{
	return part_sims_[p.first]->get(p.second);
}

void HierarchicalSim::setPort(const HierarchicalSim::FlatPort &p)
{
	if (Structural::isExternal(p))
		stateCache_.at(p.second) = true;
	else
		part_sims_[p.first]->set(p.second);
}

std::vector<const ISingle *> HierarchicalSim::getIFaces(const Entity &entity) const
{
	return entity.interface().flat();
}

}
