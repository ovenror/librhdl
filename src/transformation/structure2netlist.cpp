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

StructureToNetlist::StructureToNetlist() {}
StructureToNetlist::~StructureToNetlist() {}

Netlist StructureToNetlist::execute(const Structure &source) const
{
	Netlist netlist(source.entity(), &source, source.timing());
	to_netlist_internal(source, netlist);
	netlist.removeDisconnectedVertices();
	return netlist;
}

void StructureToNetlist::to_netlist_internal(const Structure &structure, Netlist &target) const
{
	std::vector <Netlist::Interface> parts_nl_interfaces;
	auto &elements = structure.elements();

	parts_nl_interfaces.reserve(elements.size());

	auto iter=elements.begin();

	/*
	 * generate structure vertices and interface
	 */
	parts_nl_interfaces.emplace_back();
	auto &structure_nl_iface = parts_nl_interfaces.back();

	for (auto *iface : (*iter) -> interface().flat()) {
		auto v = target.graph_.addVertex();
		structure_nl_iface[iface] = v;
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
		parts_nl_interfaces.push_back (part_netlist -> copyInto(target));
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
		assert (!port -> needs_closing());

		VertexRef persisting = parts_nl_interfaces
				.at(port -> element())
				.at(&port -> iface());
		++port;

		for (; port != connection.end(); ++port)
			connect(*port, persisting, parts_nl_interfaces, target);
	}

	//std::cerr << "exported NLInterface looks like:" << std::endl;
	//std::cerr << Netlist::InterfaceToString(parts_nl_interfaces[0]);

	target.interface_ = std::move(structure_nl_iface);
}

void StructureToNetlist::connect(
		const Port &p, VertexRef persisting,
		std::vector <Netlist::Interface> &parts_nlis,
		Netlist &target) const
 {
	auto victim = parts_nlis.at(p.element()).at(&p.iface());

	assert (victim != persisting);

	if (p.needs_closing())
		oneway(victim, persisting, p.iface().direction(), target);
	else
		merge(victim, persisting, parts_nlis, target);
}

void StructureToNetlist::merge(netlist::VertexRef victim,
	netlist::VertexRef persisting,
	std::vector<netlist::Netlist::Interface> &parts_nlis,
	netlist::Netlist &target) const
{
	for (auto &nli : parts_nlis) {
		for (auto kv : nli) {
			if (kv.second == victim)
				nli[kv.first] = persisting;
		}
	}

	target.graph_.eat(persisting, victim);

}

void StructureToNetlist::oneway(
		netlist::VertexRef victim, netlist::VertexRef persisting,
		SingleDirection dir, netlist::Netlist &target) const
{
	if (dir == SingleDirection::OUT)
		target.createOneway(victim, persisting);
	else
		target.createOneway(persisting, victim);
}

}
