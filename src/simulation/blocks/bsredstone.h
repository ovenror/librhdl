#ifndef BSREDSTONE_H
#define BSREDSTONE_H

#include "blocksim.h"

namespace rhdl {

class BSRedstone : public BlockSim
{
public:
	BSRedstone(Queue &q);

	void init_internal(const SimMap &smap, const Vec &pos, const Blocks &blocks) override;

	Block type() const {return Block::REDSTONE;}
	bool update_internal(char value) override;
};

}

#endif // BSREDSTONE_H
