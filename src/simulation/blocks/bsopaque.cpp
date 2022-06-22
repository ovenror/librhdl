#include "bsopaque.h"

namespace rhdl::blocks {

BSOpaque::BSOpaque(Queue &q) : BlockSim(q)
{

}

void BSOpaque::init_internal(const BlockSim::SimMap &smap, const BlockSim::Vec &pos, const Blocks &blocks)
{
	CVec top = blocks.cabove(pos);

	if (top.first) {
		const Vec &theTop = top.second;
		add(smap, theTop, Block::REDSTONE);
		add(smap, theTop, Block::TORCH);
	}

	for (int dir = 0; dir < 4; ++dir) {
		CVec side = blocks.csidestep(pos, (Direction) dir);

		if (!side.first)
			continue;

		Vec theSide = side.second;

		const Block &block = blocks[theSide];

		switch (block) {
		case Block::REPEATER:
			if (block.orientation() != dir)
				continue;
			[[fallthrough]];
		case Block::REDSTONE:
			addExisting(smap, theSide);
			[[fallthrough]];
		default:
			continue;
		case Block::TORCH:
			break;
		}

		Blocks::CVec belo = Blocks::cbelow(theSide);

		if (block.orientation() == (Direction) dir)
			add(smap, theSide, Block::TORCH);
	}
}

void BSOpaque::propagate() const
{
	//std::cerr << "  from " << *this << " propagate... " << std::endl;
	for (auto &psim : next_) {
		if (psim -> type() == Block::REDSTONE && value_ < 16)
			continue;

		psim -> update(value_);
	}
}

bool BSOpaque::update_internal(char value)
{
	if (value <= value_)
		return false;

	value_ = value;
	return true;
}

}

