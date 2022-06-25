#include "bstorch.h"
#include "representation/blocks/blocks.h"

namespace rhdl::blocks {

BSTorch::BSTorch(Queue &q) : BSDelay(q)
{
	resetOnAfterSwitch();
}

void BSTorch::init_internal(const BlockSim::SimMap &smap, const Vec &pos, const Blocks &blocks)
{
	add(smap, blocks.above(pos), Block::OPAQUE);
	add(smap, below(pos), Block::REDSTONE);

	for (int i = 0; i < 4; ++i) {
		Direction dir = static_cast<Direction>(i);

		CVec cside = blocks.csidestep(pos, dir);

		if (!cside.first)
			continue;

		Vec side = cside.second;

		add(smap, side, Block::REDSTONE);

		if (blocks[side].orientation() != dir)
			continue;

		add(smap, side, Block::REPEATER);
	}
}

void BSTorch::resetOnAfterSwitch()
{
	on_after_switch_ = true;
}

void BSTorch::updateOnAfterSwitch(char value)
{
	if (value)
		on_after_switch_ = false;
}

}
