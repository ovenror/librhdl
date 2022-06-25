/*
 * marray.h
 *
 *  Created on: Jun 23, 2022
 *      Author: ovenror
 */

#ifndef SRC_UTIL_MARRAY_H_
#define SRC_UTIL_MARRAY_H_

#include <boost/multi_array.hpp>

namespace marray {

enum Axis {
	Y=0, X=1, Z=2
};

using index_t = boost::multi_array<int, 3>::index;
using dimensionality_t = decltype(boost::multi_array<int, 3>::dimensionality);
using range = boost::multi_array_types::index_range;
template <dimensionality_t DIM> using fullIndex = boost::array<index_t, DIM>;

}


#endif /* SRC_UTIL_MARRAY_H_ */
