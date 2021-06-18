#ifndef BGTREE_H
#define BGTREE_H

#include "transformation/typedtransformation.h"
#include "representation/netlist/netlist.h"
#include "representation/blocks/blocks.h"
#include "treemodel.h"
#include <vector>

namespace rhdl {

class BGTree : public TypedTransformation<netlist::Netlist, blocks::Blocks>
{
public:
	BGTree();

	virtual blocks::Blocks execute(const netlist::Netlist &source) const override;

	static boost::multi_array<char, 2> project(blocks::Blocks::Cuboid blocks);

private:
	static char project(blocks::Blocks::Line line, bool shortCut);
};

std::ostream &operator<<(std::ostream &os, const boost::multi_array<char, 2> &ascii_img);

}


#endif // BGTREE_H
