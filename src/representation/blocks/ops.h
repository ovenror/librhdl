/*
 * ops.h
 *
 *  Created on: Jun 23, 2022
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_BLOCKS_OPS_H_
#define SRC_REPRESENTATION_BLOCKS_OPS_H_

#include "types.h"

namespace rhdl {
namespace blocks {

template <class DimGE3>
inline Vec dimensions(const DimGE3 &blocks);
Vec2 dimensions(const Wall &wall);
inline index_t length(const Line &line);
inline FullIndex<Line> dimensions(const Line &line);

template <class DimGE3>
inline Block &index(DimGE3 &blocks, const Vec &v);

template <class DimGE3>
inline const Block &index(const DimGE3 &blocks, const Vec &v);

template <class DimGE3>
std::vector<Vec> validSides(const DimGE3 &blocks, const Vec &origin);

template <class DimGE3>
std::array<CVec, 4> allSides(const DimGE3 &blocks, const Vec &origin);

template <class DimGE3>
CVec csidestep(const DimGE3 &blocks, Vec origin, Direction direction);

template <class DimGE3>
inline Vec sidestep(const DimGE3 &blocks, Vec origin, Direction direction);

inline Vec above(Vec origin);
inline Vec below(Vec origin);
template <class DimGE3> inline CVec cabove(const DimGE3 &blocks, Vec origin);
inline CVec cbelow(Vec origin);

Vec toGlobal(const Cuboid &blocks, Vec v);

Line slice1(Line line, index_t position, index_t length = 0);
Line slice1(Wall blocks, Vec2 position, Axis axis, index_t length = 0);
Line slice1(Cuboid blocks, Vec position, Axis axis, index_t length = 0);
Line slice1(Container &blocks, Vec position, Axis axis, index_t length = 0);
ConstLine slice1(const Container &blocks, Vec position, Axis axis, index_t length = 0);

Wall slice2(Wall wall, Vec2 position, Vec2 dimensions= {0,0});
Wall slice2(Cuboid blocks, Vec position, Axis normal, Vec2 dimensions = {0,0});
Wall slice2(Container &blocks, Vec position, Axis normal, Vec2 dimensions = {0,0});

Cuboid slice3(Cuboid blocks, Vec position, Vec dimensions = {0,0,0});
Cuboid slice3(Container &blocks, Vec position, Vec dimensions = {0,0,0});
ConstCuboid slice3(const Container &blocks, Vec position, Vec dimensions = {0,0,0});

void project2(Cuboid blocks, Axis axis, Wall target);
void project2(Container &blocks, Axis axis, Wall target);

void fill(Line space, Block value);
void fill(Wall space, Block value);
void fill(Cuboid space, Block value);
void fill(Container &space, Block value);

std::ostream &operator<<(std::ostream &os, const Wall &blocks);
std::ostream &operator<<(std::ostream &os, Block block);
std::ostream &operator<<(std::ostream &os, const Vec &v);

/*
 * inline implementations
 */

template<class DimGE3>
inline Vec dimensions(const DimGE3 &blocks)
{
	auto dims = blocks.shape();
	return {(index_t) dims[0], (index_t) dims[1], (index_t) dims[2]};
}

inline index_t length(const Line &line)
{
	 return (index_t) line.shape()[0];
}

inline FullIndex<Line> dimensions(const Line &line)
{
	 return {length(line)};
}

template<class DimGE3>
inline Block& index(DimGE3 &blocks, const Vec &v)
{
	 return blocks[v[0]][v[1]][v[2]];
}

template<class DimGE3>
inline const Block& index(const DimGE3 &blocks, const Vec &v)
{
	 return blocks[v[0]][v[1]][v[2]];
}

template<class DimGE3>
inline Vec sidestep(const DimGE3 &blocks, Vec origin,
		Direction direction)
{
	return csidestep(blocks, origin, direction).second;
}

inline Vec above(Vec origin)
{
	origin[Axis::Y] += 1;
	return origin;
}

inline Vec below(Vec origin)
{
	origin[Axis::Y] -= 1;
	return origin;
}

template <class DimGE3>
inline CVec cabove(const DimGE3 &blocks, Vec origin)
{
	Vec result = above(origin);
	return {result[Axis::Y] < dimensions(blocks)[Axis::Y], result};
}

inline CVec cbelow(Vec origin)
{
	Vec result = below(origin);
	return {result[Axis::Y] >= 0, result};
}

} /* namespace blocks */
} /* namespace rhdl */

#endif /* SRC_REPRESENTATION_BLOCKS_OPS_H_ */
