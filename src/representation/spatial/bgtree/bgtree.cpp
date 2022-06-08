#include "bgtree.h"
#include "representation/netlist/netlist.h"
#include "interface/isingle.h"
#include "node.h"
#include "util/iterable.h"
#include "simulation/blockssim.h"
#include "entity/entity.h"

#include <fstream>
#include <sstream>

namespace rhdl {

using netlist::Netlist;
using netlist::VertexRef;
using netlist::EdgeRef;

using blocks::Blocks;
using blocks::Block;

static std::vector<const ISingle *> ifilter(const Netlist::Interface &nli, Interface::Direction dir)
{
	std::vector<const ISingle *> result;

	for (auto &kv : nli) {
		const ISingle *iface = kv.first;

		if (iface -> direction() == dir)
			result.push_back(iface);
	}

	return result;
}

BGTree::BGTree() {}

Blocks BGTree::execute(const Netlist &source) const
{
	const Netlist *pnetlist = &source;
	const Entity &entity = source.entity();
	unsigned int count = 0;

	while (true) {
		const Netlist &netlist = *pnetlist;

		//std::ofstream dotfile;
		//std::stringstream dotfilename;
		//dotfilename << "netlist" << count << ".dot";
		//dotfile.open(dotfilename.str().c_str());
		//dotfile << netlist.graph_;
		//dotfile.close();

		auto bottomIFaces = ifilter(netlist.interface_, Interface::Direction::IN);
		auto topIFaces = ifilter(netlist.interface_, Interface::Direction::OUT);

		TreeModel model(netlist, bottomIFaces, topIFaces);
		Blocks target(entity, &netlist, netlist.timing());

		model.computeSpatial();
		target.resize({4, model.width(), model.height()});
		model.toBlocks(target.slice3({0,0,0}));
		model.toInterface(target.interface());

		//std::cerr << project(target.slice3({0,0,0}));

		model.createSegments();
		auto assessment = model.assessLinks(target);

		if (!model.hasBrokenLinks(assessment)) {
			return target;
		}

		auto unfixedConnections = model.fixBrokenLinks(assessment, target);

		if (unfixedConnections.empty()) {
			target.breakTiming();
			assert (!model.hasBrokenLinks(model.assessLinks(target)));
			return target;
		}
		else
			assert (model.hasBrokenLinks(model.assessLinks(target)));

		++count;

		Netlist restricted = model.splitConnections(unfixedConnections, netlist);
		pnetlist = &entity.addRepresentation(std::move(restricted));
	}
}


static bool maybeShortcut(const Blocks::Cuboid &blocks, Blocks::Vec invBlockPosStart)
{
	using blocks::Direction;

	std::array<Blocks::CVec, 2> lr;

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
			Blocks::CVec &side = lr[idx];

			if (!side.first)
				continue;

			Blocks::Vec theSide = side.second;

			assert (Blocks::cabove(blocks, theSide).first);
			assert (Blocks::cbelow(theSide).first);

			Blocks::Vec abov = Blocks::above(theSide);
			Blocks::Vec belo = Blocks::below(theSide);

			if (Blocks::index(blocks, belo) != Block::REDSTONE)
				return false;

			if (Blocks::index(blocks, theSide) == Block::OPAQUE) {
				return Blocks::index(blocks, abov) != Block::REDSTONE;
			}

			side = Blocks::csidestep(blocks, theSide, dir);
		}
	}

	return false;
}

boost::multi_array<char, 2> BGTree::project(Blocks::Cuboid blocks)
{
	Blocks::index_t width = Blocks::dimensions(blocks)[1];
	Blocks::index_t height = Blocks::dimensions(blocks)[2];
	boost::multi_array<char, 2> result(boost::extents[width][height]);

	for (Blocks::index_t x = 0; x < width; ++x) {
		for (Blocks::index_t y = 0; y < height; ++y) {
			Blocks::Vec invBlockPosStart({2, x, y});

			result[x][y] = project(Blocks::slice1(blocks, {0, x, y}, (Axis) 0), maybeShortcut(blocks, invBlockPosStart));
		}
	}

	return result;
}

char BGTree::project(Blocks::Line line, bool shortCut)
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
		case Block::TORCH: return 'o';
		case Block::UNSET: return ' ';
		default: assert(0);
	}

	return 'E';
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
