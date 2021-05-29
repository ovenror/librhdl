#ifndef MULTI_ARRAY_SLICING_H
#define MULTI_ARRAY_SLICING_H

#include <boost/multi_array.hpp>

namespace marray {

enum Axis {
	Y=0, X=1, Z=2
};

using index_t = boost::multi_array<int, 3>::index;
using dimensionality_t = decltype(boost::multi_array<int, 3>::dimensionality);
using range = boost::multi_array_types::index_range;
template <dimensionality_t DIM> using fullIndex = boost::array<index_t, DIM>;

template <Axis axis, Axis axis_idx, dimensionality_t totaldim>
struct IndexCutter1 {
	static index_t cut(fullIndex<totaldim> position, index_t length0, index_t length1)
	{
		std::ignore = length0;
		std::ignore = length1;
		return position[axis_idx];
	}
};

template <Axis axis, dimensionality_t totaldim>
struct IndexCutter1<axis, axis, totaldim> {
	static range cut(fullIndex<totaldim> position, index_t length0, index_t length1)
	{
		std::ignore = length1;

		if (length0 == 0)
			return range().start(position[axis]);

		return range(position[axis], position[axis] + length0);
	}
};

template <Axis normal, Axis axis_idx>
struct LengthDisc {enum {WHICH = -1};};

template <Axis normal, Axis axis_idx>
constexpr index_t getLength(index_t length0, index_t length1) {
	assert(normal != axis_idx);

	if (axis_idx == 0)
		return length0;

	if (axis_idx == 2)
		return length1;

	if (normal == 0) {
		return length0;
	}
	else {
		assert (normal == 2);
		return length1;
	}
}

template <Axis normal, Axis axis_idx, dimensionality_t totaldim>
struct IndexCutter2 {
	static range cut(fullIndex<totaldim> position, index_t length0, index_t length1)
	{
		index_t length = getLength<normal, axis_idx>(length0, length1);

		if (length == 0)
			return range().start(position[axis_idx]);

		return range(position[axis_idx], position[axis_idx] + length);
	}
};

template <Axis normal, dimensionality_t totaldim>
struct IndexCutter2<normal, normal, totaldim> {
	static index_t cut(fullIndex<totaldim> position, index_t length0, index_t length1)
	{
		std::ignore = length0;
		std::ignore = length1;
		return position[normal];
	}
};

template <template <Axis, Axis, dimensionality_t> class cutter, Axis axis, Axis axis_idx, dimensionality_t totaldim>
struct IndexMaker {
	static auto make(fullIndex<totaldim> position, index_t length0, index_t length1) {
		return IndexMaker<cutter, axis, (Axis) (axis_idx - 1), totaldim>::make(position, length0, length1)[cutter<axis, axis_idx, totaldim>::cut(position, length0, length1)];
	}
};

template <template <Axis, Axis, dimensionality_t> class cutter, Axis axis, dimensionality_t totaldim>
struct IndexMaker<cutter, axis, (Axis) 0, totaldim> {
	static auto make(fullIndex<totaldim> position, index_t length0, index_t length1) {
		return boost::indices[cutter<axis, (Axis) 0, totaldim>::cut(position, length0, length1)];
	}
};

template <template <Axis, Axis, dimensionality_t> class cutter, Axis saxis, dimensionality_t DIM>
struct IndexSwitcher {
	static auto zawitsch(fullIndex<DIM> position, Axis axis, index_t length0, index_t length1)
	{
		if (axis == saxis)
			return IndexMaker<cutter, saxis, (Axis) (DIM-1), DIM>::make(position, length0, length1);
		else
			return IndexSwitcher<cutter, (Axis) (saxis-1), DIM>::zawitsch(position, axis, length0, length1);
	}
};

template <template <Axis, Axis, dimensionality_t> class cutter, dimensionality_t DIM>
struct IndexSwitcher<cutter, (Axis) 0, DIM> {
	static auto zawitsch(fullIndex<DIM> position, Axis axis, index_t length0, index_t length1) {
		assert (axis == 0);
		return IndexMaker<cutter, (Axis) 0, (Axis) (DIM-1), DIM>::make(position, length0, length1);
	}
};

template <template <Axis, Axis, dimensionality_t> class cutter, dimensionality_t DIM>
inline auto mkIndex(fullIndex<DIM> position, Axis axis, index_t length0, index_t length1 = 0)
{
	assert (axis < DIM);
	return IndexSwitcher<cutter, (Axis) (DIM-1), DIM>::zawitsch(position, axis, length0, length1);
}

template <dimensionality_t DIM>
inline auto mkIndex1(fullIndex<DIM> position, Axis axis, index_t length = 0)
{
	return mkIndex<IndexCutter1, DIM>(position, axis, length);
}

inline auto mkIndex11(index_t position, index_t length = 0)
{
	return mkIndex1<1>({position}, (Axis) 0, length);
}

template <dimensionality_t DIM>
inline auto mkIndex2(fullIndex<DIM> position, Axis normal, fullIndex<2> dimensions = {0,0})
{
	return mkIndex<IndexCutter2, DIM>(position, normal, dimensions[0], dimensions[1]);
}

inline auto mkIndex22(fullIndex<2> position, fullIndex<2> dimensions = {0,0})
{
	return IndexMaker<IndexCutter2, (Axis) 2, (Axis) 1, 2>::make(position, dimensions[0], dimensions[1]);
}

}

#endif // MULTI_ARRAY_SLICING_H
