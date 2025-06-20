/*
 * ops.cpp
 *
 *  Created on: Jun 23, 2022
 *      Author: ovenror
 */

#include "ops.h"

#include "util/marray_slicing_3D.h"
#include "util/marray_eiterator.h"
#include "util/util.h"
#include "util/staticswitch.h"

#include <array>
#include <cassert>

namespace rhdl {
namespace blocks {

template <class T>
constexpr bool isConstNoRef =
		std::is_const_v<T> ||
		std::is_same_v<T, ConstLine> ||
		std::is_same_v<T, ConstWall> ||
		std::is_same_v<T, ConstCuboid>;

template <class T>
constexpr bool isConst = isConstNoRef<std::remove_reference_t<T>>;


using Setup = staticswitch::Setup<staticswitch::Class, staticswitch::Class>;

template <class SWITCH, class DEFAULT, class... CASES>
using Switch = Setup::Switch<SWITCH, DEFAULT, CASES...>;

template <class CASE, class RESULT>
using Case = Setup::Case<CASE, RESULT>;

template <class T>
using View =
		Switch<std::remove_reference_t<T>, T,
			Case<Container, Cuboid>,
			Case<const Container, ConstCuboid>
			>;

template <class VIEW>
using Constify =
		Switch<VIEW, VIEW,
			Case<Line, ConstLine>,
			Case<Wall, ConstWall>,
			Case<Cuboid, ConstCuboid>
			>;

template <class VIEW, bool CONST>
using CondConstify = std::conditional_t<CONST, Constify<VIEW>, VIEW>;


Vec2 dimensions(const Wall &wall)
{
	auto dims = wall.shape();
	return {(index_t) dims[0], (index_t) dims[1]};
}


template <class DimGE3>
std::vector<Vec> validSides(const DimGE3 &blocks, const Vec &origin)
{
	std::vector<Vec> result;

	for (const auto &side : allSides(blocks, origin)) {
		if (side.first)
			result.push_back(side.second);
	}

	return result;
}

template std::vector<Vec> validSides(const Cuboid &blocks, const Vec &origin);
template std::vector<Vec> validSides(const Container &blocks, const Vec &origin);

template <class DimGE3>
std::array<CVec, 4> allSides(const DimGE3 &blocks, const Vec &origin)
{
	std::array<CVec, 4> result;

	for (unsigned int dir = 0; dir < 4 ; ++dir) {
		result[dir] = csidestep(blocks, origin, (Direction) dir);
	}

	return result;
}

template std::array<CVec, 4> allSides(const Cuboid &blocks, const Vec &origin);
template std::array<CVec, 4> allSides(const Container &blocks, const Vec &origin);

template <class DimGE3>
CVec csidestep(const DimGE3 &blocks, Vec origin, Direction direction)
{
	int idx = (direction & 1) + 1;
	index_t step = 1 - (direction & 2);
	CVec result = {true, origin};
	index_t &ordinate = result.second[idx];
	ordinate += step;
	result.first = (ordinate >= 0) && (ordinate < dimensions(blocks)[idx]);
	return result;
}

template CVec csidestep(const Cuboid &blocks, Vec origin, Direction direction);
template CVec csidestep(const ConstCuboid &blocks, Vec origin, Direction direction);
template CVec csidestep(const Container &blocks, Vec origin, Direction direction);

Vec toGlobal(const Cuboid &blocks, Vec v)
{
	Vec result;

	for (unsigned int dim = 0; dim < 3; ++dim)
		result[dim] = v[dim] + blocks.index_bases()[dim];

	//std::cerr << "local " << v << " to global " << result << std::endl;

	return result;
}

template <class DimGE2>
static CondConstify<Line, isConst<DimGE2>> slice1_internal(
		DimGE2 &&blocks, FullIndex<typename std::remove_reference<DimGE2>::type> position,
		Axis axis, index_t length)
{
	return std::forward<DimGE2>(blocks)[marray::mkIndex1<std::remove_reference<DimGE2>::type::dimensionality>(position, axis, length)];
}

Line slice1(Line line, index_t position, index_t length)
{
	return line[marray::mkIndex11(position, length)];
}

Line slice1(Wall blocks, Vec2 position, Axis axis, index_t length)
{
	return slice1_internal(blocks, position, axis, length);
}

Line slice1(Cuboid blocks, Vec position, Axis axis, index_t length)
{
	return slice1_internal(blocks, position, axis, length);
}

ConstLine slice1(ConstCuboid blocks, Vec position, Axis axis, index_t length)
{
	return slice1_internal(blocks, position, axis, length);
}

Line slice1(Container &blocks, Vec position, Axis axis, index_t length)
{
	return slice1_internal(blocks, position, axis, length);
}

ConstLine slice1(const Container &blocks, Vec position, Axis axis, index_t length)
{
	return slice1_internal(blocks, position, axis, length);
}

template<class DimGE3>
static Wall slice2_internal(DimGE3 &&blocks, Vec position, Axis normal, Vec2 dimensions)
{
	return std::forward<DimGE3>(blocks)[marray::mkIndex2(position, normal, dimensions)];
}

Wall slice2(Wall wall, Vec2 position, Vec2 dimensions)
{
	return wall[marray::mkIndex22(position, dimensions)];
}

Wall slice2(Cuboid blocks, Vec position, Axis normal, Vec2 dimensions)
{
	return slice2_internal(blocks, position, normal, dimensions);
}

Wall slice2(Container &blocks, Vec position, Axis normal, Vec2 dimensions)
{
	return slice2_internal(blocks, position, normal, dimensions);
}

template<class DimGE3>
static CondConstify<Cuboid, isConst<DimGE3>> slice3_internal(
			DimGE3 &&blocks, Vec position, Vec dimensions)
{
	return std::forward<DimGE3>(blocks)[boost::indices
			[dimensions[0]?range(position[0], position[0]+dimensions[0]):range().start(position[0])]
			[dimensions[1]?range(position[1], position[1]+dimensions[1]):range().start(position[1])]
			[dimensions[2]?range(position[2], position[2]+dimensions[2]):range().start(position[2])]];
}

Cuboid slice3(Cuboid blocks, Vec position, Vec dimensions)
{
	return slice3_internal(blocks, position, dimensions);
}

Cuboid slice3(Container &blocks, Vec position, Vec dimensions)
{
	return slice3_internal(blocks, position, dimensions);
}

ConstCuboid slice3(const Container &blocks, Vec position, Vec dimensions)
{
	return slice3_internal(blocks, position, dimensions);
}

void project2(Container &blocks, Axis axis, Wall target)
{
	project2(slice3(blocks, {0,0,0}), axis, target);
}

void project2(Cuboid blocks, Axis axis, Wall target)
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

template<class MARRAY>
static void fill_internal(MARRAY &&space, Block value)
{
	std::fill(marray::ebegin(std::forward<MARRAY>(space)), marray::eend(std::forward<MARRAY>(space)), value);
}

void fill(Line space, Block value)
{
	fill_internal(space, value);
}

void fill(Wall space, Block value)
{
	fill_internal(space, value);
}

void fill(Cuboid space, Block value)
{
	fill_internal(space, value);
}

void fill(Container &space, Block value)
{
	fill_internal(space, value);
}

std::ostream &operator<<(std::ostream &os, const Wall &blocks)
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

std::ostream &operator<<(std::ostream &os, const Vec &v)
{
	os << "(" << v[0] << "," << v[1] << "," << v[2] << ")";
	return os;
}



} /* namespace blocks */

} /* namespace rhdl */
