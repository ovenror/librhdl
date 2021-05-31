#include <interface/visitors/vertexmerger.h>
#include "transformation/structure2netlist.h"
#include "representation/netlist/netlist.h"
#include "representation/structural/structural.h"
#include "entity/entity.h"

namespace rhdl {

StructureToNetlist::StructureToNetlist()
{
}

Netlist StructureToNetlist::execute(const Structural &source) const
{
	Netlist netlist(source.entity(), &source, source.timing());
	to_netlist_internal(source, netlist);
	netlist.removeDisconnectedVertices();
	return netlist;
}

void StructureToNetlist::to_netlist_internal(const Structural &structure, Netlist &target) const
{
	std::vector <Netlist::Interface> parts_nl_interfaces;

	// dummy interface for ourself
	parts_nl_interfaces.push_back(Netlist::Interface());

	//std::cerr << "EComposite, net to netlist..." << std::endl;
	//std::cerr << "generate inner netlists:" << std::endl;

	/*
	 * generate inner netlists
	 */
	auto iter=structure.parts().begin();
	++iter;
	for (; iter != structure.parts().end(); ++iter)
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
	for (auto connection : structure.flatConnections())
	//for (auto connection : connections_) // works too
	{
		VertexMerger vm(target.graph_, parts_nl_interfaces, connection.first.first, connection.second.first);
		vm.go_visit(connection.first.second, connection.second.second);
	}

	//std::cerr << "exported NLInterface looks like:" << std::endl;
	//std::cerr << Netlist::InterfaceToString(parts_nl_interfaces[0]);

	target.interface_ = std::move(parts_nl_interfaces[0]);
}

}
