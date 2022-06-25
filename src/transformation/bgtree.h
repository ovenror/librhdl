#ifndef BGTREE_H
#define BGTREE_H

#include "transformation/typedtransformation.h"
#include "representation/blocks/types.h"
#include <vector>

namespace rhdl {

namespace spatial {
class TreeModel;
}

namespace blocks {
class Blocks;
}

class BGTree : public TypedTransformation<spatial::TreeModel, blocks::Blocks>
{
public:
	BGTree();

	virtual std::unique_ptr<blocks::Blocks> execute(const spatial::TreeModel &source) const override;

	static boost::multi_array<char, 2> project(blocks::Cuboid blocks);

private:
	static char project(blocks::Line line, bool shortCut);
};

std::ostream &operator<<(std::ostream &os, const boost::multi_array<char, 2> &ascii_img);

}


#endif // BGTREE_H
