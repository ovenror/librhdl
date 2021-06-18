#include "blocksim.h"
#include <iostream>

namespace rhdl::blocks {

BlockSim::BlockSim(Queue &q)
	: value_(0), queue_(q)
{}

void BlockSim::init(const BlockSim::SimMap &smap, const BlockSim::Vec &pos, const Blocks &blocks)
{
	debug_pos_ = pos;
	init_internal(smap, pos, blocks);
}

void BlockSim::update(char value)
{
	//std::cerr << *this << " update " << (int) value << std::endl;

	if (update_internal(value))
		queue_.push(this);
}

void BlockSim::add(const BlockSim::SimMap &smap, const BlockSim::Vec &pos)
{
	if (smap.find(pos) == smap.end())
		return;

	addExisting(smap, pos);
}

void BlockSim::add(const BlockSim::SimMap &smap, const BlockSim::Vec &pos, Block btype)
{    
	if (smap.find(pos) == smap.end())
		return;

	if (smap.at(pos) -> type() == btype)
		addExisting(smap, pos);
}

void BlockSim::addExisting(const BlockSim::SimMap &smap, const BlockSim::Vec &pos)
{
	next_.push_back(smap.at(pos).get());
}

void BlockSim::propagate() const
{
	for (auto &bs : next_)
		bs -> update(value_);
}

std::ostream &operator<<(std::ostream &os, const BlockSim &bs)
{
	os << typeid(bs).name() << "(" << (void*) &bs << ")";
	os << " p:" << bs.debug_pos_[0];
	os << "," << bs.debug_pos_[1];
	os << "," << bs.debug_pos_[2];

	return os;
}

}

