#include <representation/structural/connection.h>
#include <representation/structural/structure.h>
#include "hierarchicalsim.h"

#include "entity/entity.h"

#include <tuple>
#include <memory>
#include <iostream>

namespace rhdl::structural {

HierarchicalSim::HierarchicalSim(const Structure &structure, bool use_behavior) :
	structure_(structure), stepping_(false)
{
	using behavioral::TimedBehavior;

	auto ipart = structure.elements().begin();
	++ipart; //skip ourself
	part_sims_.push_back(this);

	for (;ipart != structure.elements().end(); ++ipart) {
		const Entity &part = **ipart;
		const Representation *partRep = nullptr;

		if (use_behavior) {
			partRep = part.getRepresentation(TimedBehavior::ID);

			if (!partRep || partRep -> timing() != part.defaultTiming())
				partRep = part.getRepresentation(Structure::ID);
		}
		else {
			partRep = part.getRepresentation(Structure::ID);

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
// FIXME: it shouldn't be necessary anymore to call propagateConnectionsOnce() in a loop
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
	for (const Connection &c : structure_.connections()) {
		if (propagateConnection(c))
			changed = true;
	}
	return changed;
}

bool HierarchicalSim::propagateConnection(const Connection &c)
{
	bool any_output = false;
	bool all_inputs = true;

	for (const Port &port : c) {
		if (!getPortInput(port))
			all_inputs = false;

		if (getPortOutput(port))
			any_output = true;

		if (any_output && !all_inputs)
			break;
	}

	if (!any_output || all_inputs)
		return false;

	for (const Port &port : c)
		setPort(port);

	return true;
}

bool HierarchicalSim::getPortInput(const Port &p) const
{
	if (p.isExternal())
		return stateCache_.at(&p.iface());
	else
		return part_sims_[p.element()]->getInput(&p.iface());
}


bool HierarchicalSim::getPortOutput(const Port &p) const
{
	return part_sims_[p.element()]->get(&p.iface());
}

void HierarchicalSim::setPort(const Port &p)
{
	if (p.isExternal())
		stateCache_.at(&p.iface()) = true;
	else
		part_sims_[p.element()]->set(&p.iface());
}

std::vector<const ISingle *> HierarchicalSim::getIFaces(const Entity &entity) const
{
	return entity.interface().flat();
}

}
