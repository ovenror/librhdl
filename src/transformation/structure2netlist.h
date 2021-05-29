#ifndef STRUCTURETONETLIST_H
#define STRUCTURETONETLIST_H

#include "typedtransformation.h"
#include "representation/netlist/netlist.h"

namespace rhdl {

class Structural;
class Netlist;

class StructureToNetlist : public TypedTransformation<Structural, Netlist>
{
public:
	StructureToNetlist();

	virtual Netlist execute(const Structural &source) const override;

protected:
	void to_netlist_internal(const Structural &structure, Netlist &target) const;
};

}

#endif // STRUCTURETONETLIST_H
