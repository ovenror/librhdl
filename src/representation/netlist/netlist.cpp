#include "netlist.h"
#include "interface/isingle.h"
#include "simulation/netlistsim.h"

#include "util/iterable.h"

#include <array>
#include <forward_list>
#include <unordered_map>

namespace rhdl::netlist {

Netlist::Netlist(
		const Entity &entity,
		Graph graph, InterfaceMap ifaceMap,
		const Representation *parent, const Timing *timing)
	:
		MappedRepresentation<Netlist, VertexRef>(
				entity, parent, timing, std::move(ifaceMap)),
		graph_(std::move(graph))
{}


Netlist::Netlist(
		const Entity &entity, const Representation *parent,
		const Timing *timing)
	:
	  MappedRepresentation<Netlist, VertexRef>(entity, parent, timing)
{}

Netlist::Netlist(const Netlist &source, std::forward_list<VertexRef> toSplit)
	: Netlist(source)
{
	assert (!toSplit.empty());

	for (const auto &vertex : toSplit) {
		splitVertex(vertex);
	}

	breakTiming();
}

std::unique_ptr<Simulator> Netlist::makeSimulator(bool use_behavior) const
{
	std::ignore = use_behavior;
	return std::make_unique<NetlistSim>(*this);
}

std::string Netlist::InterfaceToString(const InterfaceMap &ifaceMap)
{
	std::string result;
	for (auto kv : ifaceMap)
		result += std::string("    * ") + (std::string) *kv.first + "->" + std::to_string(kv.second) + "\n";
	return result;
}

Netlist::InterfaceMap Netlist::copyInto(Graph &target) const
{
	InterfaceMap result;

	auto vmap = target.absorb(graph_);
	for (auto kv : ifaceMap()) {
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

	for (auto &kv : ifaceMap_) {
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

		ifaceMap_[iface] = collector;
	}

	removeVertex(vertex);
}

void Netlist::removeVertex(VertexRef vertex)
{
	remapInterface(graph_.removeVertex(vertex));
}

void Netlist::remapInterface(const std::map<VertexRef, VertexRef> &vertexMap)
{
	for (auto &kv : ifaceMap_) {
		kv.second = vertexMap.at(kv.second);
	}
}


size_t Netlist::iCount(VertexRef v, SingleDirection dir) const
{
	size_t result = 0;

	for (const auto [iface, iv] : ifaceMap_) {
		if (iv == v)
			++result;
	}

	return result;
}

bool Netlist::iHas(VertexRef v, SingleDirection dir) const
{
	for (const auto [iface, iv] : ifaceMap_) {
		if (iv == v)
			return true;
	}

	return false;
}

size_t Netlist::iCountIn(VertexRef v) const
{
	return graph_.countIn(v) + iCount(v, SingleDirection::IN);
}

size_t Netlist::iCountOut(VertexRef v) const
{
	return graph_.countOut(v) + iCount(v, SingleDirection::OUT);
}

bool Netlist::iHasIn(VertexRef v) const
{
	return graph_.countIn(v) || iHas(v, SingleDirection::IN);
}

bool Netlist::iHasOut(VertexRef v) const
{
	return graph_.countOut(v) || iHas(v, SingleDirection::OUT);
}
}
