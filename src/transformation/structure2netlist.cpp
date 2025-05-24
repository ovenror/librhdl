#include "structure2netlist.h"

#include "representation/structural/structure.h"
#include "representation/netlist/netlist.h"

#include "entity/entity.h"

namespace rhdl {

using netlist::Netlist;
using netlist::VertexRef;

using structural::Structure;
using structural::ElementIdx;
using structural::Port;

StructureToNetlist::StructureToNetlist() : TypedTransformation("Structure2Netlist") {}
StructureToNetlist::~StructureToNetlist() {}

std::unique_ptr<Netlist> StructureToNetlist::execute(
		const Structure &source, const std::string &result_name) const
{
	Netlist::Graph graph;

	auto ifaceMap = to_netlist_internal(source, graph);
	Netlist::remap(ifaceMap, graph.removeDisconnectedVertices());

	return std::make_unique<Netlist>(
			source.entity(), std::move(graph), std::move(ifaceMap),
			&source, source.timing(), result_name);
}

Netlist::InterfaceMap StructureToNetlist::to_netlist_internal(
		const Structure &structure, Netlist::Graph &target) const
{
	std::vector <Netlist::InterfaceMap> parts_ifaceMaps;
	auto &elements = structure.elements();

	parts_ifaceMaps.reserve(elements.size());

	auto iter=elements.begin();

	/*
	 * generate structure vertices and interface
	 */
	parts_ifaceMaps.emplace_back();
	auto &ifaceMap = parts_ifaceMaps.back();

	for (auto *iface : (*iter) -> interface().flat()) {
		auto v = target.addVertex();
		ifaceMap[iface] = v;
	}

	/*
	 * generate inner netlists
	 */
	++iter;

	for (; iter != elements.end(); ++iter)
	{
		auto entity = *iter;

		//std::cerr << "generate inner netlist for a " << typeid(*entity).name() << std::endl;
		const Netlist *part_netlist = entity -> getRepresentation<Netlist>();
		assert(part_netlist);
		parts_ifaceMaps.push_back (part_netlist -> copyInto(target));
	}

	/*
	 * connect inner entities' netlists
	 */
	//std::cerr << "connect netlists:" << std::endl;
	for (auto connection : structure.connections())
	//for (auto connection : connections_) // works too
	{
		auto port = connection.begin();
		assert (port != connection.end());

		VertexRef persisting = parts_ifaceMaps
				.at(port -> element())
				.at(&port -> iface());
		bool open = (target.isInternal(persisting));


		++port;

		for (; port != connection.end(); ++port)
			connect(*port, persisting, parts_ifaceMaps, target, open);
	}

	//std::cerr << "exported NLInterface looks like:" << std::endl;
	//std::cerr << Netlist::InterfaceToString(parts_nl_interfaces[0]);

	return std::move(ifaceMap);
}

void StructureToNetlist::connect(
		const Port &p, VertexRef persisting,
		std::vector <Netlist::InterfaceMap> &parts_nlis,
		Netlist::Graph &target, bool &open) const
 {
	auto victim = parts_nlis.at(p.element()).at(&p.iface());

	assert (victim != persisting);

	if (target.isInternal(victim)) {
		if (open) {
			oneway(victim, persisting, p.iface().direction(), target);
			return;
		} else {
			open = true;
		}
	}

	merge(victim, persisting, parts_nlis, target);
}

void StructureToNetlist::merge(netlist::VertexRef victim,
	netlist::VertexRef persisting,
	std::vector<netlist::Netlist::InterfaceMap> &parts_nlis,
	netlist::Netlist::Graph &target) const
{
	for (auto &nli : parts_nlis) {
		for (auto kv : nli) {
			if (kv.second == victim)
				nli[kv.first] = persisting;
		}
	}

	target.eat(persisting, victim);

}

void StructureToNetlist::oneway(
		netlist::VertexRef victim, netlist::VertexRef persisting,
		SingleDirection dir, netlist::Netlist::Graph &target) const
{
	if (dir == SingleDirection::OUT)
		target.createOneway(victim, persisting);
	else
		target.createOneway(persisting, victim);
}

}
