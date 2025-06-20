/*
 * types.h
 *
 *  Created on: Jun 23, 2022
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_BLOCKS_TYPES_H_
#define SRC_REPRESENTATION_BLOCKS_TYPES_H_

#include "block.h"
#include <util/marray.h>

namespace rhdl::blocks {

using Axis = marray::Axis;
using Container = boost::multi_array<Block, 3>;
using Line = boost::array_view_gen<Container,1>::type;
using ConstLine = boost::const_array_view_gen<Container,1>::type;
using Wall = boost::array_view_gen<Container,2>::type;
using ConstWall = boost::const_array_view_gen<Container,2>::type;
using Cuboid = boost::array_view_gen<Container,3>::type;
using ConstCuboid = boost::const_array_view_gen<Container,3>::type;
using index_t = marray::index_t;
using range = boost::multi_array_types::index_range;
template <class MARRAY> using FullIndex = boost::array<index_t, MARRAY::dimensionality>;
using Vec = FullIndex<Container>;
using Vec2 = FullIndex<Wall>;
using BlockRef = Vec;
using CVec = std::pair<bool, Vec>;

}

#endif /* SRC_REPRESENTATION_BLOCKS_TYPES_H_ */
