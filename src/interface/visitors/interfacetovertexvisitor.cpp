#include "interfacetovertexvisitor.h"

namespace rhdl {


InterfaceToVertexVisitor::InterfaceToVertexVisitor(
		Netlist::Graph &netlist, std::vector<Netlist::Interface> &parts,
		PartIdx source, PartIdx destination)
	:
	  netlist_(netlist), parts_(parts),
	  source_(source), destination_(destination)
{
}


void InterfaceToVertexVisitor::visit(const ISingle &i1,const ISingle &i2)
{
	//std::cerr << "setting vertex of " << (std::string) i1 << " to vertex of " << (std::string) i2 << std::endl;

	auto &sparts = parts_[source_];
	auto &dparts = parts_[destination_];

	VertexRef persisting = sparts.at(&i1);
	auto existing = dparts.find(&i2);

	if (existing == dparts.end()) {
		assert(destination_ == containingPartIdx);
		dparts[&i2] = persisting;
	}
	else {
		replace(existing -> second, persisting);
	}
}

void InterfaceToVertexVisitor::replace(VertexRef v1, VertexRef v2)
{

	if (v1 == v2)
	{
		//std::cerr << "both vertices are the same..." << std::endl;
		return;
	}

	// we are producing fully bi-partite connection graphs here
	// TODO: check beforehand, if we are really bi-partitely connected
	for (Netlist::Interface &nl : parts_) {
		for (auto kv : nl) {
			if (kv.second == v1)
				nl[kv.first] = v2;
		}
	}

	netlist_.eat(v2, v1);
}

}
