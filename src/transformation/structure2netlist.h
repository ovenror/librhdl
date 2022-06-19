#ifndef STRUCTURETONETLIST_H
#define STRUCTURETONETLIST_H

#include "typedtransformation.h"
#include "representation/netlist/netlist.h"

#include "interface/direction.h"

namespace rhdl {

namespace netlist { class Netlist; }

namespace structural {
class Structure;
class Port;
}

class StructureToNetlist : public TypedTransformation<structural::Structure, netlist::Netlist>
{
public:
	StructureToNetlist();
	virtual ~StructureToNetlist();

	virtual netlist::Netlist execute(
			const structural::Structure &source) const override;

protected:
	netlist::Netlist::InterfaceMap to_netlist_internal(
			const structural::Structure &structure,
			netlist::Netlist::Graph &target) const;

private:
	void connect(
			const structural::Port &p, netlist::VertexRef v,
			std::vector <netlist::Netlist::InterfaceMap> &,
			netlist::Netlist::Graph &target, bool &open) const;

	void merge(
			netlist::VertexRef victim, netlist::VertexRef persisting,
			std::vector <netlist::Netlist::InterfaceMap> &,
			netlist::Netlist::Graph &target) const;

	void oneway(
			netlist::VertexRef, netlist::VertexRef, SingleDirection,
			netlist::Netlist::Graph &target) const;

};

}

#endif // STRUCTURETONETLIST_H
