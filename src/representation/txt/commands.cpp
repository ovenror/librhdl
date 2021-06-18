/*
 * commands.cpp
 *
 *  Created on: May 23, 2021
 *      Author: ovenror
 */

#include "commands.h"

#include "simulation/simulator.h"

#include "representation/blocks/blocks.h"

#include <cassert>
#include <array>

namespace rhdl {

using blocks::Block;
using blocks::Blocks;

namespace txt {

Commands::Commands(const Blocks &blocks)
	: RepresentationBase(blocks.entity(), &blocks, blocks.timing()),
	  blocks_(blocks), translation_{0,0,0}
{}

Commands::~Commands() {}

void Commands::moveTo(Blocks::Vec pos) {
	translation_ = pos;
}

static std::array<std::string, 4> facings{"west", "north", "east", "south"};

void Commands::out(std::ostream& os) const {
	Blocks::Vec mcpos, pos{0,0,0};
	std::copy(translation_.begin(), translation_.end(), mcpos.begin());
	

	while (pos[0] < blocks_.dimensions()[0]) {
		os << "setblock " << mcpos[marray::X] << " ";
		os << mcpos[marray::Y] << " " << mcpos[marray::Z] << " ";
	
		const auto &block = blocks_[pos];
		const auto &dir = block.orientation();

		switch (block) {
		case Block::UNSET:
		case Block::TRANSPARENT: os << "minecraft:air"; break;
		case Block::OPAQUE: os << "minecraft:quartz_block"; break;
		case Block::REDSTONE: 
			os << "minecraft:redstone_wire";
			os << "[north=up, west=up, south=up, east=up]";
			break;
		case Block::TORCH:
			os << "minecraft:redstone_wall_torch[facing=";
			os << facings[dir^2] << "]";
			break;
		case Block::REPEATER:
			os << "minecraft:repeater[facing=";
			os << facings[dir] << "]";
			break;
		default: assert(0);
		}

		os << " replace" << std::endl;

		++pos[2];
		++mcpos[2];

		for (auto axis : {2, 1}) {
			if (pos[axis] >= blocks_.dimensions()[axis]) {
				++pos[axis - 1];
				++mcpos[axis - 1];
				pos[axis] = 0;
				mcpos[axis] = translation_[axis];
			}
		}
	}
}

std::ostream& operator <<(std::ostream& os, const Commands& commands) {
	commands.out(os);
	return os;
}

} /* namespace txt */
} /* namespace rhdl */
