#include "bsrepeater.h"
#include "representation/blocks/blocks.h"

namespace rhdl::blocks {

BSRepeater::BSRepeater(Queue &q) : BSDelay(q)
{
	resetOnAfterSwitch();
}

void BSRepeater::init_internal(const BlockSim::SimMap &smap, const Vec &pos, const Blocks &blocks)
{
	Direction orientation = blocks[pos].orientation();
	CVec front = blocks.csidestep(pos, orientation);

	if (!front.first)
		return;


	switch (blocks[front.second]) {
	case Block::REDSTONE:
	case Block::REPEATER:
	case Block::OPAQUE:
		break;
	default:
		//std::cerr << *this << " to " << front.second << " " << blocks[front.second] << std::endl;
		assert(0);
	}

	add(smap, front.second, Block::OPAQUE);
	add(smap, front.second, Block::REDSTONE);

	if (blocks[front.second].orientation() != orientation)
		return;

	add(smap, front.second, Block::REPEATER);
}

void BSRepeater::resetOnAfterSwitch()
{
	on_after_switch_ = false;
}

void BSRepeater::updateOnAfterSwitch(char value)
{
	//std::cerr << *this << " updated " << (int) value << std::endl;

	if (value)
		on_after_switch_ = true;
}

}
