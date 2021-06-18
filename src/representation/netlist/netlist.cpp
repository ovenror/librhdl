#include "netlist.h"
#include "interface/isingle.h"
#include "simulation/netlistsim.h"

#include "util/iterable.h"

#include <forward_list>

namespace rhdl::netlist {

Netlist::Netlist(
		const Entity &entity, const Representation *parent,
		const Timing *timing)
	:
	  RepresentationBase<Netlist>(entity, parent, timing), ready_(false)
{}

std::unique_ptr<Simulator> Netlist::makeSimulator(bool use_behavior) const
{
	std::ignore = use_behavior;
	return std::make_unique<NetlistSim>(*this);
}

std::string Netlist::InterfaceToString(const Netlist::Interface &nli)
{
	std::string result;
	for (auto kv : nli)
		result += std::string("    * ") + (std::string) *kv.first + "->" + std::to_string(kv.second) + "\n";
	return result;
}

Netlist::Interface Netlist::copyInto(Netlist &target) const
{
	Netlist::Interface result;

	auto vmap = target.graph_.absorb(graph_);
	for (auto kv : interface_) {
		result[kv.first] = vmap.at(kv.second);
	}

	return result;
}

void Netlist::removeDisconnectedVertices()
{
	remapInterface(graph_.removeDisconnectedVertices());
}


void Netlist::splitVertex(VertexRef vertex)
{
	auto inEdges = Iterable(graph_.inEdges(vertex));
	auto outEdges = Iterable(graph_.outEdges(vertex));

	for (EdgeRef outEdge : outEdges) {
		VertexRef collector = graph_.addVertex();

		for (EdgeRef inEdge : inEdges)
			graph_.connect(graph_.source(inEdge), collector);

		graph_.connect(collector, graph_.target(outEdge));
	}

	for (auto &kv : interface_) {
		if (kv.second != vertex)
			continue;

		const ISingle *iface = kv.first;

		auto collector = graph_.addVertex();

		if (iface -> direction() == rhdl::Interface::Direction::OUT) {
			for (EdgeRef inEdge : inEdges)
				graph_.connect(graph_.source(inEdge), collector);
		}

		if (iface -> direction() == rhdl::Interface::Direction::IN) {
			for (EdgeRef outEdge : outEdges)
				graph_.connect(collector, graph_.target(outEdge));
		}

		interface_[iface] = collector;
	}

	removeVertex(vertex);
}

void Netlist::removeVertex(VertexRef vertex)
{
	remapInterface(graph_.removeVertex(vertex));
}

void Netlist::remapInterface(const std::map<VertexRef, VertexRef> &vertexMap)
{
	for (auto &kv : interface_) {
		kv.second = vertexMap.at(kv.second);
	}
}

void Netlist::createOneway(VertexRef from, VertexRef to)
{
	auto middle = graph_.addVertex();
	graph_.connect(from, middle);
	graph_.connect(middle, to);
}

}
