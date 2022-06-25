#include "bsredstone.h"
#include "representation/blocks/blocks.h"

namespace rhdl::blocks {

BSRedstone::BSRedstone(Queue &q) : BlockSim(q)
{

}

void BSRedstone::init_internal(const BlockSim::SimMap &smap, const Vec &pos, const Blocks& blocks)
{
	auto sides = blocks.allSides(pos);

	add(smap, cbelow(pos).second, Block::OPAQUE);

	std::array<bool, 4> hasRedstone = {false, false, false, false};

	CVec abov = blocks.cabove(pos);
	bool opaqueAbove = (abov.first && blocks[abov.second] == Block::OPAQUE);

	for (unsigned int direction = 0; direction < 4; ++direction) {
		CVec &side = sides[direction];

		if (!side.first)
			continue;

		Vec theSide = side.second;

		if (blocks[theSide].type() == Block::REPEATER) {
			if (blocks[theSide].orientation() == static_cast<Direction>(direction)) {
				addExisting(smap, theSide);
				hasRedstone[direction] = true;
			}

			continue;
		}

		std::array<CVec, 3> column = {
			blocks.cabove(theSide),
			side,
			cbelow(theSide)
		};

		unsigned int z = opaqueAbove ? 1 : 0;

		for (; z < column.size(); ++z) {
			const CVec &v = column[z];

			if (!v.first)
				continue;

			const Vec &theV = v.second;

			if (blocks[theV] == Block::OPAQUE && z == 1)
				break;

			if (blocks[theV] != Block::REDSTONE)
				continue;

			hasRedstone[direction] = true;
			add(smap, theV);
			break;
		}
	}

	for (unsigned int direction = 0; direction < 4; ++direction) {
		CVec &side = sides[direction];

		if (!side.first)
			continue;

		Vec theSide = side.second;

		if (blocks[theSide] != Block::OPAQUE)
			continue;

		unsigned int jdx = 1;
		for (; jdx <=3; jdx += 2)
			if (hasRedstone[(direction + jdx) & 3])
				break;

		if (jdx <= 3)
			continue;

		add(smap, theSide);
	}
}

bool BSRedstone::update_internal(char value)
{
	char next = value - 1;

	if (value_ >= next)
		return false;

	value_ = next;
	return true;
}

}
