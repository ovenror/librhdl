#ifndef INTERFACEEXPOSUREVISITOR_H
#define INTERFACEEXPOSUREVISITOR_H

#include "interfacedoublevisitor.h"

#include "representation/structural/structural.h" //FIXME: Only needed for PartIdx?
#include "representation/netlist/netlist.h"

#include <iostream>

namespace rhdl {

class VertexMerger : public InterfaceDoubleVisitor<true>
{
public:
	using Super = InterfaceDoubleVisitor<true>;

	VertexMerger(
			Netlist::Graph &netlist, std::vector<Netlist::Interface> &parts,
			PartIdx source, PartIdx destination);

	void visit(const ISingle &i1, const ISingle &i2)  override;

protected:
	virtual void replace(VertexRef v1, VertexRef v2);

	Netlist::Graph &netlist_;
	std::vector<Netlist::Interface> &parts_;
	PartIdx source_;
	PartIdx destination_;
};

}

#endif // INTERFACEEXPOSUREVISITOR_H
