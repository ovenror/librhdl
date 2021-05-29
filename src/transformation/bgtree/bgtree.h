#ifndef BGTREE_H
#define BGTREE_H

#include "transformation/typedtransformation.h"
#include "representation/netlist/netlist.h"
#include "treemodel.h"
#include <vector>

namespace rhdl {

class BGTree : public TypedTransformation<Netlist, Blocks>
{
public:
	BGTree();

	virtual Blocks execute(const Netlist &source) const override;

	static boost::multi_array<char, 2> project(Blocks::Cuboid blocks);

private:
	static char project(Blocks::Line line, bool shortCut);
};

std::ostream &operator<<(std::ostream &os, const boost::multi_array<char, 2> &ascii_img);

}


#endif // BGTREE_H
