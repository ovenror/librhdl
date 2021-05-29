#ifndef BSOPAQUE_H
#define BSOPAQUE_H

#include "blocksim.h"

namespace rhdl {

class BSOpaque : public BlockSim
{
public:
	BSOpaque(Queue &q);

	void init_internal(const SimMap &smap, const Vec &pos, const Blocks& blocks) override;
	void propagate() const override;

	Block type() const {return Block::OPAQUE;}
	bool update_internal(char value) override;
};

}

#endif // BSOPAQUE_H
