#include "blocks.h"
#include "simulation/blockssim.h"
#include <cassert>

namespace rhdl::blocks {

Blocks::Blocks(const Entity &entity, const Representation *parent,
			   const Timing *timing)
	:
	  RepresentationBase<Blocks>(entity, parent, timing)
{}

Blocks::~Blocks() {
}

std::unique_ptr<Simulator> Blocks::makeSimulator(bool use_behavior) const
{
	std::ignore = use_behavior;
	return std::make_unique<BlocksSim>(*this);
}

Blocks::Vec2 Blocks::dimensions(const Blocks::Wall &wall)
{
	auto dims = wall.shape();
	return {(index_t) dims[0], (index_t) dims[1]};
}

Blocks::Vec Blocks::dimensions() const
{
	return dimensions(the_blocks_);
}

void Blocks::resize(Vec dims)
{
	the_blocks_.resize(boost::extents[dims[0]][dims[1]][dims[2]]);
}

Blocks::Line Blocks::slice1(Line line, Blocks::index_t position, Blocks::index_t length)
{
	return line[marray::mkIndex11(position, length)];
}

Blocks::Line Blocks::slice1(Blocks::Wall blocks, Blocks::Vec2 position, Axis axis, Blocks::index_t length)
{
	return slice1_internal(blocks, position, axis, length);
}

Blocks::Line Blocks::slice1(Blocks::Cuboid blocks, Blocks::Vec position, Axis axis, Blocks::index_t length)
{
	return slice1_internal(blocks, position, axis, length);
}

Blocks::Line Blocks::slice1(Blocks::Container &blocks, Blocks::Vec position, Axis axis, Blocks::index_t length)
{
	return slice1_internal(blocks, position, axis, length);
}

Blocks::Wall Blocks::slice2(Blocks::Wall wall, Vec2 position, Vec2 dimensions)
{
	return wall[marray::mkIndex22(position, dimensions)];
}

Blocks::Wall Blocks::slice2(Blocks::Cuboid blocks, Blocks::Vec position, Axis normal, Blocks::Vec2 dimensions)
{
	return slice2_internal(blocks, position, normal, dimensions);
}

Blocks::Wall Blocks::slice2(Blocks::Container &blocks, Blocks::Vec position, Axis normal, Blocks::Vec2 dimensions)
{
	return slice2_internal(blocks, position, normal, dimensions);
}

template <class DimGE2>
Blocks::Line Blocks::slice1_internal(DimGE2 &&blocks, Blocks::FullIndex<typename std::remove_reference<DimGE2>::type> position, Axis axis, Blocks::index_t length)
{
	return std::forward<DimGE2>(blocks)[marray::mkIndex1<std::remove_reference<DimGE2>::type::dimensionality>(position, axis, length)];
}

template<class CuboidOrContainer>
Blocks::Wall Blocks::slice2_internal(CuboidOrContainer &&blocks, Vec position, Axis normal, Vec2 dimensions)
{
	return std::forward<CuboidOrContainer>(blocks)[marray::mkIndex2(position, normal, dimensions)];
}

Blocks::Cuboid Blocks::slice3(Cuboid blocks, Vec position, Vec dimensions)
{
	return slice3_internal(blocks, position, dimensions);
}

Blocks::Cuboid Blocks::slice3(Blocks::Container &blocks, Blocks::Vec position, Blocks::Vec dimensions)
{
	return slice3_internal(blocks, position, dimensions);
}

void Blocks::project2(Blocks::Container &blocks, Axis axis, Blocks::Wall target)
{
	project2(slice3(blocks, {0,0,0}), axis, target);
}

void Blocks::fill(Blocks::Line space, Block value)
{
	fill_internal(space, value);
}

void Blocks::fill(Blocks::Wall space, Block value)
{
	fill_internal(space, value);
}

void Blocks::fill(Blocks::Cuboid space, Block value)
{
	fill_internal(space, value);
}

void Blocks::fill(Blocks::Container &space, Block value)
{
	fill_internal(space, value);
}

void Blocks::fill(Block value)
{
	fill(the_blocks_, value);
}

template<class DimGE3>
Blocks::Vec Blocks::dimensions(const DimGE3 &blocks)
{
	auto dims = blocks.shape();
	return {(index_t) dims[0], (index_t) dims[1], (index_t) dims[2]};
}

void Blocks::project2(Cuboid blocks, Axis axis, Wall target)
{
	auto dim = dimensions(blocks);

	unsigned di = 0;
	for (auto tdim : dimensions(target)) {
		if (di == (unsigned) axis) ++di;
		assert(tdim == dim[di]);
		++di;
	}

	for (index_t layer_index = 0; layer_index < dim[axis]; ++layer_index) {
		Vec position({0,0,0});
		position[axis] = layer_index;
		Wall layer = slice2(blocks, position, axis);

		auto isrc = marray::ebegin(layer);
		auto itgt = marray::ebegin(target);

		while (isrc != marray::eend(layer)) {
			assert (itgt != marray::eend(target));

			switch (*isrc) {
			case Block::OPAQUE:
			case Block::REDSTONE:
			case Block::TORCH:
				*itgt = *isrc;
				break;
			default:
				if (*itgt == Block::UNSET)
					*itgt = *isrc;
			}

			++isrc;
			++itgt;
		}
		assert (itgt == marray::eend(target));
	}
}

template<class CuboidOrContainer>
Blocks::Cuboid Blocks::slice3_internal(CuboidOrContainer &&blocks, Blocks::Vec position, Blocks::Vec dimensions)
{
	return std::forward<CuboidOrContainer>(blocks)[boost::indices
			[dimensions[0]?range(position[0], position[0]+dimensions[0]):range().start(position[0])]
			[dimensions[1]?range(position[1], position[1]+dimensions[1]):range().start(position[1])]
			[dimensions[2]?range(position[2], position[2]+dimensions[2]):range().start(position[2])]];
}

std::ostream &operator<<(std::ostream &os, const Blocks::Wall &blocks)
{
	for (const auto &column : blocks) {
		for (auto block : column) {
			os << block;
		}
		os << std::endl;
	}

	return os;
}

std::ostream &operator<<(std::ostream &os, Block block)
{
	switch (block) {
	case Block::OPAQUE: os << "X"; break;
	case Block::REDSTONE: os << "+"; break;
	case Block::TORCH: os << "O"; break;
	default: os << " ";
	}

	return os;
}

template<class MARRAY>
void Blocks::fill_internal(MARRAY &&space, Block value)
{
	std::fill(marray::ebegin(std::forward<MARRAY>(space)), marray::eend(std::forward<MARRAY>(space)), value);
}

Blocks::Vec Blocks::above(Blocks::Vec origin)
{
	origin[Axis::Y] += 1;
	return origin;
}

Blocks::Vec Blocks::below(Blocks::Vec origin)
{
	origin[Axis::Y] -= 1;
	return origin;
}

Blocks::CVec Blocks::csidestep(Blocks::Vec origin, Direction direction) const
{
	return csidestep(the_blocks_, origin, direction);
}

Blocks::CVec Blocks::cabove(Blocks::Vec origin) const
{
	return cabove(the_blocks_, origin);
}

Blocks::Vec Blocks::toGlobal(const Blocks::Cuboid &blocks, Blocks::Vec v)
{
	Vec result;

	for (unsigned int dim = 0; dim < 3; ++dim)
		result[dim] = v[dim] + blocks.index_bases()[dim];

	//std::cerr << "local " << v << " to global " << result << std::endl;

	return result;
}

template <class DimGE3>
Blocks::CVec Blocks::cabove(const DimGE3 &blocks, Blocks::Vec origin)
{
	Vec result = above(origin);
	return {result[Axis::Y] < dimensions(blocks)[Axis::Y], result};
}

template Blocks::CVec Blocks::cabove(const Blocks::Cuboid &blocks, Blocks::Vec origin);


Blocks::CVec Blocks::cbelow(Blocks::Vec origin)
{
	Vec result = below(origin);
return {result[Axis::Y] >= 0, result};
}

std::vector<Blocks::Vec> Blocks::validSides(const Blocks::Vec &origin) const
{
	return validSides(the_blocks_, origin);
}

std::array<Blocks::CVec, 4> Blocks::allSides(const Blocks::Vec &origin) const
{
	return allSides(the_blocks_, origin);
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

template <class DimGE3>
std::array<Blocks::CVec, 4> Blocks::allSides(const DimGE3 &blocks, const Blocks::Vec &origin)
{
	std::array<CVec, 4> result;

	for (unsigned int dir = 0; dir < 4 ; ++dir) {
		result[dir] = csidestep(blocks, origin, (Direction) dir);
	}

	return result;
}

template std::array<Blocks::CVec, 4> Blocks::allSides(const Blocks::Cuboid &blocks, const Blocks::Vec &origin);


template <class DimGE3>
std::vector<Blocks::Vec> Blocks::validSides(const DimGE3 &blocks, const Blocks::Vec &origin)
{
	std::vector<Vec> result;

	for (const auto &side : allSides(blocks, origin)) {
		if (side.first)
			result.push_back(side.second);
	}

	return result;
}

template std::vector<Blocks::Vec> Blocks::validSides(const Blocks::Cuboid &blocks, const Blocks::Vec &origin);


template <class DimGE3>
Blocks::CVec Blocks::csidestep(const DimGE3 &blocks, Blocks::Vec origin, Direction direction)
{
	int idx = (direction & 1) + 1;
	index_t step = 1 - (direction & 2);
	CVec result = {true, origin};
	index_t &ordinate = result.second[idx];
	ordinate += step;
	result.first = (ordinate >= 0) && (ordinate < dimensions(blocks)[idx]);
	return result;
}

template Blocks::CVec Blocks::csidestep(const Blocks::Cuboid &blocks, Blocks::Vec origin, Direction direction);

std::ostream &operator<<(std::ostream &os, const Blocks::Vec &v)
{
	os << "(" << v[0] << "," << v[1] << "," << v[2] << ")";
	return os;
}


}
