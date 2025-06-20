#include "bgtree.h"

#include "representation/spatial/tree/treemodel.h"
#include "interface/isingle.h"
#include "simulation/blockssim.h"
#include "entity/entity.h"
#include "util/iterable.h"

#include <fstream>
#include <sstream>

namespace rhdl {

using netlist::Netlist;
using netlist::VertexRef;
using netlist::EdgeRef;

using blocks::Blocks;
using blocks::Block;
using spatial::TreeModel;

BGTree::BGTree() : TypedTransformation("BGTree") {}

std::unique_ptr<Blocks> BGTree::execute(
		const TreeModel &source, const std::string &result_name) const
{
	auto result = std::make_unique<Blocks>(source.entity(), &source, source.timing(), result_name);
	auto &target = *result;

	target.resize({4, source.width(), source.height()});
	source.toBlocks(target.slice3({0,0,0}));
	source.toInterface(const_cast<Blocks::InterfaceMap &>(target.ifaceMap()));

	auto assessment = source.assessLinks(target);
	assert (!source.hasBrokenLinks(assessment));

	LOG(LOG_REP) << project(target.slice3({0,0,0}));
	auto dim = target.dimensions();
	LOG(LOG_REP) << dim[0] << " x " << dim[1] << " x " << dim[2] << std::endl;

	return result;
}

/* Check whether the inverter at invBlockPosStart could be shortcut-connected,
 * i.e. powering a wire directly underneath instead of a wire in front of it.
 */
static bool maybeShortcut(const blocks::ConstCuboid &blocks, blocks::Vec invBlockPosStart)
{
	using blocks::Direction;

	// positions walking to the left and the right from the inverter
	std::array<blocks::CVec, 2> lr;

	// first, initialize to inverter position
	for (unsigned int idx = 0; idx <= 1; ++idx) {
		lr[idx].first = true;
		lr[idx].second = invBlockPosStart;
	}

	while (lr[0].first || lr[1].first) {
		for (unsigned int idx = 0; idx <= 1; ++idx) {
			//std::cerr << "really vec " << idx << ": ";
			//std::cerr << lr[idx].second[0] << ",";
			//std::cerr << lr[idx].second[1] << ",";
			//std::cerr << lr[idx].second[2] << std::endl;

			Direction dir = (Direction) (idx << 1);
			blocks::CVec &side = lr[idx];

			if (!side.first)
				continue;

			blocks::Vec theSide = side.second;

			assert (blocks::cabove(blocks, theSide).first);
			assert (blocks::cbelow(theSide).first);

			blocks::Vec abov = blocks::above(theSide);
			blocks::Vec belo = blocks::below(theSide);

			/* FIXME: Does the perpendicular wire below really have to
			 * take the whole breadth? */
			if (blocks::index(blocks, belo) != Block::REDSTONE)
				return false;

			if (blocks::index(blocks, theSide) == Block::OPAQUE) {
				return blocks::index(blocks, abov) != Block::REDSTONE;
			}

			// now do the walking step (dir = left or right)
			side = blocks::csidestep(blocks, theSide, dir);
		}
	}

	return false;
}

boost::multi_array<char, 2> BGTree::project(blocks::ConstCuboid blocks)
{
	blocks::index_t width = blocks::dimensions(blocks)[1];
	blocks::index_t height = blocks::dimensions(blocks)[2];
	boost::multi_array<char, 2> result(boost::extents[width][height]);

	for (blocks::index_t x = 0; x < width; ++x) {
		for (blocks::index_t y = 0; y < height; ++y) {
			blocks::Vec invBlockPosStart({2, x, y});

			result[x][y] = project(blocks::slice1(blocks, {0, x, y}, (blocks::Axis) 0), maybeShortcut(blocks, invBlockPosStart));
		}
	}

	return result;
}

char BGTree::project(const blocks::ConstLine &line, bool shortCut)
{
	using blocks::Direction;
	using blocks::RIGHT;
	using blocks::FORWARD;
	using blocks::LEFT;
	using blocks::BACKWARD;

	if (line[1] == Block::REPEATER || line[3] == Block::REPEATER) {
		Direction overt = FORWARD;
		Direction ohoriz = LEFT;

		bool horizontal = line[3] == Block::REPEATER;
		bool vertical = line[1] == Block::REPEATER;

		bool both = horizontal && vertical;

		if (vertical)
			overt = line[1].orientation();

		if (horizontal)
			ohoriz = line[3].orientation();

		if (both) {
			switch (ohoriz) {
			case RIGHT:
				switch (overt) {
				case FORWARD: return 'J';
				case BACKWARD: return 'L';
				default: return 'E';
				}
			case LEFT:
				switch (overt) {
				case FORWARD: return '7';
				case BACKWARD: return 'F';
				default: return 'E';
				}
			default: return 'E';
			}
		} else {
			Direction o = vertical ? overt : ohoriz;

			switch (o) {
			case FORWARD : return '>';
			case LEFT : return '^';
			case RIGHT : return 'v';
			case BACKWARD : return '<';
			default:
				return 'E';
			}
		}

		return 'E';
	}

	if (line[1] == Block::REDSTONE) {
		if (line[2] == Block::OPAQUE)
			return line[3] == Block::REDSTONE ? '+' : '#';

		if (line[2] == Block::TORCH) {
			switch (line[2].orientation()) {
			case FORWARD : return '0';
			case LEFT : return '0';
			case RIGHT : return '0';
			default:
				return 'E';
			}
		}

		if (shortCut)
			return ':';

		return '-';
	}

	if (line[3] == Block::REDSTONE) {
		assert (line[1] != Block::REDSTONE);
		return '|';
	}

	switch (line[2]) {
		case Block::REDSTONE: return '*';
		case Block::OPAQUE: return 'X';
		case Block::TORCH: return '0';
	}

	if (line[1] == Block::OPAQUE) {
		return 'x';
	}

	if (line[1] == Block::TORCH) {
		return 'o';
	}

	for (blocks::index_t h=0; h<4; ++h)
		if (line[h] != Block::UNSET)
			return 'E';

	return ' ';
}

std::ostream &operator<<(std::ostream &os, const boost::multi_array<char, 2> &ascii_img)
{
	for (const auto &sub : ascii_img) {
		for (char c : sub) {
			os << c;
		}
		os << std::endl;
	}

	return os;
}


}
