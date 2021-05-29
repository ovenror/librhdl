#ifndef BLOCKS_H
#define BLOCKS_H

#include "block.h"
#include "direction.h"

#include "representation/representationbase.h"

#include "util/marray_slicing_3D.h"
#include "util/marray_eiterator.h"

#include <vector>
#include <array>
#include <iostream>
#include <map>

namespace rhdl {

using Axis = marray::Axis;

class ISingle;
class Timing;

class Blocks : public RepresentationBase<Blocks>
{
public:
	using Container = boost::multi_array<Block, 3>;
	using Line = boost::array_view_gen<Container,1>::type;
	using Wall = boost::array_view_gen<Container,2>::type;
	using Cuboid = boost::array_view_gen<Container,3>::type;
	using index_t = marray::index_t;
	using range = boost::multi_array_types::index_range;
	template <class MARRAY> using FullIndex = boost::array<index_t, MARRAY::dimensionality>;
	using Vec = FullIndex<Container>;
	using Vec2 = FullIndex<Wall>;
	using BlockRef = Vec;
	using Interface = std::map<const ISingle *, BlockRef>;
	using CVec = std::pair<bool, Vec>;

	Blocks(
			const Entity &entity, const Representation *parent,
			const Timing *timing);

	virtual ~Blocks();

	const Interface &interface() const {return interface_;}
	Interface &interface() {return interface_;}

	virtual std::unique_ptr<Simulator> makeSimulator(bool use_behavior) const override;

	template <class DimGE3>
	static Block &index(DimGE3 &blocks, const Vec &v) {return blocks[v[0]][v[1]][v[2]];}

	template <class DimGE3>
	static const Block &index(const DimGE3 &blocks, const Vec &v) {return blocks[v[0]][v[1]][v[2]];}

	Block &operator[](const Vec &v) {return index(the_blocks_, v);}
	const Block &operator[](const Vec &v) const {return index(the_blocks_, v);}

	operator const Container&() const {return the_blocks_;}

	template <class T>
	auto &operator[](T pos) {return the_blocks_[pos];}

	template <class T>
	const auto &operator[](T pos) const {return the_blocks_[pos];}

	template <class DimGE3>
	static Vec dimensions(const DimGE3 &blocks);
	static Vec2 dimensions(const Wall &wall);
	static index_t length(const Line &line) {return (index_t) line.shape()[0];}
	static FullIndex<Line> dimensions(const Line &line) {return {length(line)};}

	Vec dimensions() const;

	void resize(Vec dims);

	template <class DimGE3>
	static std::vector<Vec> validSides(const DimGE3 &blocks, const Vec &origin);

	template <class DimGE3>
	static std::array<CVec, 4> allSides(const DimGE3 &blocks, const Vec &origin);

	template <class DimGE3>
	static CVec csidestep(const DimGE3 &blocks, Vec origin, Direction direction);

	template <class DimGE3>
	static Vec sidestep(const DimGE3 &blocks, Vec origin, Direction direction)
	{
		return csidestep(blocks, origin, direction).second;
	}

	static Vec above(Vec origin);
	static Vec below(Vec origin);
	template <class DimGE3> static CVec cabove(const DimGE3 &blocks, Vec origin);
	static CVec cbelow(Vec origin);

	std::vector<Vec> validSides(const Vec &origin) const;
	std::array<CVec, 4> allSides(const Vec &origin) const;

	CVec csidestep(Vec origin, Direction direction) const;
	Vec sidestep(Vec origin, Direction direction) const {return csidestep(origin, direction).second;}
	CVec cabove(Vec origin) const;

	static Vec toGlobal(const Cuboid &blocks, Vec v);

	template <class DimGE3>
	static Vec toGlobal(const DimGE3 &blocks, Vec v);

#if 0
	Vec left(Vec origin) const;
	Vec right(Vec origin) const;
	CVec cleft(Vec origin) const;
	CVec cright(Vec origin) const;
#endif

	static Line slice1(Line line, index_t position, index_t length = 0);
	static Line slice1(Wall blocks, Vec2 position, Axis axis, index_t length = 0);
	static Line slice1(Cuboid blocks, Vec position, Axis axis, index_t length = 0);
	static Line slice1(Container &blocks, Vec position, Axis axis, index_t length = 0);

	static Wall slice2(Wall wall, Vec2 position, Vec2 dimensions= {0,0});
	static Wall slice2(Cuboid blocks, Vec position, Axis normal, Vec2 dimensions = {0,0});
	static Wall slice2(Container &blocks, Vec position, Axis normal, Vec2 dimensions = {0,0});

	static Cuboid slice3(Cuboid blocks, Vec position, Vec dimensions = {0,0,0});
	static Cuboid slice3(Container &blocks, Vec position, Vec dimensions = {0,0,0});

	static void project2(Cuboid blocks, Axis axis, Wall target);
	static void project2(Blocks::Container &blocks, Axis axis, Blocks::Wall target);

	Line slice1(Vec position, Axis axis, index_t length = 0) {return slice1(the_blocks_, position, axis, length);}
	Wall slice2(Vec position, Axis normal, Vec2 dimensions = {0,0}) {return slice2(the_blocks_, position, normal, dimensions);}
	Cuboid slice3(Vec position, Vec dimensions = {0,0,0}) {return slice3(the_blocks_, position, dimensions);}

	void project2(Axis axis, Wall target) {project2(the_blocks_, axis, target);}

	static void fill(Line space, Block value);
	static void fill(Wall space, Block value);
	static void fill(Cuboid space, Block value);
	static void fill(Container &space, Block value);

	void fill(Block value);

private:
	friend std::ostream &operator<<(std::ostream &os, const Wall &blocks);

	template <class DimGE2>
	static Line slice1_internal(DimGE2 &&blocks, Blocks::FullIndex<typename std::remove_reference<DimGE2>::type> position, Axis axis, index_t length = 0);

	template <class CuboidOrContainer>
	static Wall slice2_internal(CuboidOrContainer &&blocks, Vec position, Axis normal, Vec2 dimensions = {0,0});

	template <class CuboidOrContainer>
	static Cuboid slice3_internal(CuboidOrContainer &&blocks, Vec position, Vec dimensions);

	template <class MARRAY>

	static void fill_internal(MARRAY &&space, Block value);

	Container the_blocks_;
	Interface interface_;
};

std::ostream &operator<<(std::ostream &os, const Blocks::Wall &blocks);
std::ostream &operator<<(std::ostream &os, Block block);
std::ostream &operator<<(std::ostream &os, const Blocks::Vec &v);

#if 0
/* operator= does not work! */
template <long unsigned DIM>
boost::array<Blocks::index_t, DIM> operator+(
		const boost::array<Blocks::index_t, DIM> &lhs,
		const boost::array<Blocks::index_t, DIM> &rhs)
{
	boost::array<Blocks::index_t, DIM> result;

	for (Blocks::index_t idx = 0; idx < DIM; ++idx) {
		result[idx] = lhs[idx] + rhs[idx];
	}

	return result;
}
#endif

}

#endif // BLOCKS_H
