#ifndef BLOCKS_H
#define BLOCKS_H

#include "ops.h"
#include "direction.h"

#include "util/marray_slicing_3D.h"
#include "util/marray_eiterator.h"

#include <vector>
#include <array>
#include <iostream>
#include <map>
#include "../mappedrepresentation.h"

namespace rhdl {

class ISingle;
class Timing;

namespace blocks {

class Blocks : public MappedRepresentation<Blocks, BlockRef>
{
	static_assert(ID == RHDL_BLOCKS);
public:
	Blocks(
			const Entity &entity, const Representation *parent,
			const Timing *timing, const std::string &name);

	virtual ~Blocks();

	virtual std::unique_ptr<Simulator> makeSimulator(bool use_behavior) const override;

	Block &operator[](const Vec &v);
	const Block &operator[](const Vec &v) const;

	operator const Container&() const;

	template <class T>
	auto &operator[](T pos) {return the_blocks_[pos];}

	template <class T>
	const auto &operator[](T pos) const {return the_blocks_[pos];}

	Vec dimensions() const;
	void resize(Vec dims);

	std::vector<Vec> validSides(const Vec &origin) const;
	std::array<CVec, 4> allSides(const Vec &origin) const;

	Vec above(Vec origin) const;

	CVec csidestep(Vec origin, Direction direction) const;
	Vec sidestep(Vec origin, Direction direction) const;
	CVec cabove(Vec origin) const;

#if 0
	Vec left(Vec origin) const;
	Vec right(Vec origin) const;
	CVec cleft(Vec origin) const;
	CVec cright(Vec origin) const;
#endif

	Line slice1(Vec position, Axis axis, index_t length = 0);
	Wall slice2(Vec position, Axis normal, Vec2 dimensions = {0,0});
	Cuboid slice3(Vec position, Vec dimensions = {0,0,0});

	void project2(Axis axis, Wall target);

	void fill(Block value);

private:
	bool existsElementRef(BlockRef) override;

	Container the_blocks_;
};

inline Block& Blocks::operator [](const Vec &v)
{
	return index(the_blocks_, v);
}

inline const Block& Blocks::operator [](const Vec &v) const
{
	return index(the_blocks_, v);
}

inline Vec Blocks::above(Vec origin) const
{
	return blocks::above(origin);
}

inline Vec Blocks::sidestep(Vec origin, Direction direction) const
{
	 return csidestep(origin, direction).second;
}

inline Line Blocks::slice1(Vec position, Axis axis, index_t length)
{
	return blocks::slice1(the_blocks_, position, axis, length);
}

inline Wall Blocks::slice2(Vec position, Axis normal, Vec2 dimensions)
{
	return blocks::slice2(the_blocks_, position, normal, dimensions);
}

inline Cuboid Blocks::slice3(Vec position, Vec dimensions)
{
	return blocks::slice3(the_blocks_, position, dimensions);
}

inline void Blocks::project2(Axis axis, Wall target)
{
	blocks::project2(the_blocks_, axis, target);
}

inline void Blocks::fill(Block value)
{
	blocks::fill(the_blocks_, value);
}

inline bool Blocks::existsElementRef(BlockRef vec)
{
	for (index_t i = 0; i < BlockRef::static_size; ++i)
	{
		if (vec[i] >= dimensions()[i])
			return false;
	}

	return true;
}

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

}}

#endif // BLOCKS_H
