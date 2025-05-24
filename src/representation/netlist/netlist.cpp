#include "netlist.h"
#include "interface/isingle.h"
#include "simulation/netlistsim.h"

#include "util/iterable.h"

#include <array>
#include <forward_list>
#include <fstream>
#include <unordered_map>

namespace rhdl::netlist {

Netlist::Netlist(
		const Entity &entity,
		Graph graph, InterfaceMap ifaceMap,
		const Representation *parent, const Timing *timing,
		const std::string &name)
	:
		MappedRepresentation<Netlist, VertexRef>(
				entity, parent, timing, std::move(ifaceMap), name),
		graph_(std::move(graph))
{
	checkIfaceMap();
	initIFaceProperties();
	//dot();
	removeUnnecessaryOneways();
	//dot(".opt");
}


Netlist::Netlist(
		const Entity &entity, const Representation *parent,
		const Timing *timing, const std::string &name)
	:
	  MappedRepresentation<Netlist, VertexRef>(entity, parent, timing, name)
{}

Netlist::Netlist(
		const Netlist &source, std::forward_list<VertexRef> toSplit,
		const std::string &name)
	: MappedRepresentation(
				source.entity(), &source, source.timing(), source.ifaceMap(), name),
			graph_(source.graph())
{
	assert (!toSplit.empty());

	//horizontalSplitVertices(toSplit);
	verticalSplitVertices(toSplit);
	breakTiming();
	dot();
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
		result += std::string("    * ") + (std::string &) *kv.first + "->" + std::to_string(kv.second) + "\n";
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

void Netlist::horizontalSplitVertex(VertexRef vertex)
{
	auto inEdges = Iterable(graph_.inEdges(vertex));
	auto outEdges = Iterable(graph_.outEdges(vertex));

	for (EdgeRef outEdge : outEdges) {
		VertexRef collector = graph_.addVertex();

		for (EdgeRef inEdge : inEdges)
			graph_.connect(graph_.source(inEdge), collector);

		graph_.connect(collector, graph_.target(outEdge));
	}

	for (auto iface : graph_[vertex].ifaces_in) {
		auto collector = graph_.addVertex();

		for (EdgeRef outEdge : outEdges)
			graph_.connect(collector, graph_.target(outEdge));

		graph_[collector].ifaces_in.insert(iface);
		ifaceMap_[iface] = collector;
	}

	for (auto iface : graph_[vertex].ifaces_out) {
		auto collector = graph_.addVertex();

		for (EdgeRef inEdge : inEdges)
			graph_.connect(graph_.source(inEdge), collector);

		graph_[collector].ifaces_out.insert(iface);
		ifaceMap_[iface] = collector;
	}

	graph_[vertex].ifaces_in.clear();
	graph_[vertex].ifaces_out.clear();
	graph_.clear(vertex);
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

void Netlist::removeUnnecessaryOneways()
{
	for (auto v1 : Iterable(graph_.vertices())) {
		if (graph_.countOut(v1) == 0)
			continue;

		bool v1_extra_out = iCountOut(v1) > 1;
		std::vector<VertexRef> eligible_v2;
		std::map<VertexRef, VertexRef> eligible_v3;
		bool ok = true;

		for (auto e12 : Iterable(graph_.outEdges(v1))) {
			auto v2 = graph_.target(e12);

			/*
			 * check v1 -> v1
			 */
			if (v2 == v1) {
				ok = false;
				break;
			}

			if (graph_.countOut(v2) < 1 || iCountIn(v2) > 1)
				continue;

			assert (iCountIn(v2) == 1);

			bool v2_ok = true;

			for (auto e23 : Iterable(graph_.outEdges(v2))) {
				auto v3 = graph_.target(e23);

				if (v3 == v2 || v3 == v1) {
					ok = false;
					break;
				}

				if (iCountIn(v3) == 1) {
					eligible_v3.insert({v2, v3});
					continue;
				}

				if (!v1_extra_out && (iCountOut(v2) == 1)) {
					eligible_v3.insert({v2, v3});
					continue;
				}

				v2_ok = false;
			}

			if (v2_ok)
				eligible_v2.push_back(v2);
		}

		if (!ok)
			continue;

		for (auto [v2, v3] : eligible_v3) {
			graph_.disconnect(v2, v3);
			LOG(DEBUG) << "removing oneway: "
					<< v1 << " eat " << v3 << std::endl;
			eat(v1, v3);
		}

		for (auto v2 : eligible_v2) {
			LOG(DEBUG) << "removing oneway: clear " << v2 << std::endl;
			graph_.clear_out(v2);

			assert (graph_[v2].ifaces_in.empty());

			if (graph_[v2].ifaces_out.empty()) {
				graph_.clear_in(v2);
			}
		}
	}

	removeDisconnectedVertices();
}

size_t Netlist::iCountIn(VertexRef v) const
{
	return graph_.countIn(v) + graph_[v].ifaces_in.size();
}

size_t Netlist::iCountOut(VertexRef v) const
{
	return graph_.countOut(v) + graph_[v].ifaces_out.size();
}

bool Netlist::iHasIn(VertexRef v) const
{
	return graph_.countIn(v) || !graph_[v].ifaces_in.empty();
}

bool Netlist::iHasOut(VertexRef v) const
{
	return graph_.countOut(v) || !graph_[v].ifaces_out.empty();
}

void Netlist::initIFaceProperties()
{
	for (auto v : Iterable(graph_.vertices())) {
		graph_[v] = {};
	}

	for (auto [iface, v] : ifaceMap_) {
		switch (iface -> direction()) {
		case SingleDirection::OUT:
			graph_[v].ifaces_out.emplace(iface);
			break;
		case SingleDirection::IN:
			graph_[v].ifaces_in.emplace(iface);
			break;
		default:
			assert (0);
		}
	}
}

void Netlist::eat(VertexRef eater, VertexRef eaten)
{
	for (auto iface : graph_[eaten].ifaces_in)
		ifaceMap_[iface] = eater;

	for (auto iface : graph_[eaten].ifaces_out)
		ifaceMap_[iface] = eater;

	graph_.eat(eater, eaten);
}

void Netlist::horizontalSplitVertices(std::forward_list<VertexRef> toSplit)
{
	for (const auto &vertex : toSplit) {
		horizontalSplitVertex(vertex);
	}

	removeDisconnectedVertices();
}

void Netlist::verticalSplitVertices(std::forward_list<VertexRef> toSplit)
{
	for (const auto &vertex : toSplit) {
		verticalSplitVertex(vertex);
	}
}

void Netlist::verticalSplitVertex(VertexRef vertex)
{
	auto outEdges = Iterable(graph_.outEdges(vertex));

	auto middle = graph_.addVertex();
	auto out = graph_.addVertex();


	for (auto outEdge : outEdges) {
		graph_.connect(out, graph_.target(outEdge));
	}

	graph_.clearOutEdges(vertex);
	graph_.connect(vertex, middle);
	graph_.connect(middle, out);

	graph_[out].ifaces_out = std::move(graph_[vertex].ifaces_out);
	graph_[vertex].ifaces_out.clear();

	for (auto iface : graph_[out].ifaces_out) {
		ifaceMap_[iface] = out;
	}
}

void Netlist::compute_content(std::string &result) const
{
	std::stringstream ss;
	ss << graph_;
	result = ss.str();
}

void Netlist::dot(std::string extra) const
{
	if (logLevel >= LOG_REP) {
		std::stringstream filename;
		filename << name() << extra << ".dot";
		std::ofstream dotfile(filename.str());
		dotfile << graph_;
	}
}

}
