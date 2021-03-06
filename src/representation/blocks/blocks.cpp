#include "blocks.h"
#include "simulation/blockssim.h"
#include <cassert>

namespace rhdl::blocks {

Blocks::Blocks(const Entity &entity, const Representation *parent,
			   const Timing *timing)
	:
	  MappedRepresentation<Blocks, BlockRef>(entity, parent, timing)
{}

Blocks::~Blocks() {
}

std::unique_ptr<Simulator> Blocks::makeSimulator(bool use_behavior) const
{
	std::ignore = use_behavior;
	return std::make_unique<BlocksSim>(*this);
}

Vec Blocks::dimensions() const
{
	return blocks::dimensions(the_blocks_);
}

void Blocks::resize(Vec dims)
{
	the_blocks_.resize(boost::extents[dims[0]][dims[1]][dims[2]]);
}

CVec Blocks::csidestep(Vec origin, Direction direction) const
{
	return blocks::csidestep(the_blocks_, origin, direction);
}

CVec Blocks::cabove(Vec origin) const
{
	return blocks::cabove(the_blocks_, origin);
}

std::vector<Vec> Blocks::validSides(const Vec &origin) const
{
	return blocks::validSides(the_blocks_, origin);
}

std::array<CVec, 4> Blocks::allSides(const Vec &origin) const
{
	return blocks::allSides(the_blocks_, origin);
}

#if 0
Blocks::Vec Blocks::left(Blocks::Vec origin) const
{
  origin[Axis::X] -= 1;
  return origin;
}

Blocks::Vec Blocks::right(Blocks::Vec origin) const
{
  origin[Axis::X] += 1;
  return origin;
}

Blocks::CVec Blocks::cleft(Blocks::Vec origin) const
{
	Vec result = left(origin);
	return {result[Axis::X] >= 0, result};
}

Blocks::CVec Blocks::cright(Blocks::Vec origin) const
{
  Vec result = right(origin);
  return {result[Axis::X] < dimensions()[Axis::X], result};
}
#endif

}
