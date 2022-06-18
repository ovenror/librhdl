#ifndef BGTREE_H
#define BGTREE_H

#include "transformation/typedtransformation.h"
#include "representation/blocks/blocks.h"
#include <vector>

namespace rhdl {

namespace netlist {
class Netlist;
}

class BGTree : public TypedTransformation<spatial::TreeModel, blocks::Blocks>
{
public:
	BGTree();

	virtual blocks::Blocks execute(const spatial::TreeModel &source) const override;

	static boost::multi_array<char, 2> project(blocks::Blocks::Cuboid blocks);

private:
	static char project(blocks::Blocks::Line line, bool shortCut);
};

std::ostream &operator<<(std::ostream &os, const boost::multi_array<char, 2> &ascii_img);

}


#endif // BGTREE_H
