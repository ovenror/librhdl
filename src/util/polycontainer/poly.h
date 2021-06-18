/*
 * poly.h
 *
 *  Created on: Sep 8, 2021
 *      Author: ovenror
 */

#ifndef SRC_UTIL_POLYCONTAINER_POLY_H_
#define SRC_UTIL_POLYCONTAINER_POLY_H_

#include "polyimpl.h"
#include "polybase2.h"
#include "util/list.h"
#include "util/util.h"

namespace rhdl {
namespace polycontainer {

template <class Container, class... LINEAGE>
using Poly = PolyImpl<Container, detail::PolyBase2<
		Container,
		List<LINEAGE...>,
		typename remove_unique_ptr<typename Container::value_type>::type, LINEAGE...>>;

} /* namespace polycontainer */
} /* namespace rhdl */

#endif /* SRC_UTIL_POLYCONTAINER_POLY_H_ */
