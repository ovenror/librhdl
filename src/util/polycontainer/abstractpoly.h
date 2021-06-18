/*
 * abstractpoly.h
 *
 *  Created on: Aug 22, 2021
 *      Author: ovenror
 */

#ifndef SRC_UTIL_POLYCONTAINER_ABSTRACTPOLY_H_
#define SRC_UTIL_POLYCONTAINER_ABSTRACTPOLY_H_

#include "abstractpolyimpl.h"

namespace rhdl::polycontainer {

template <class Value, class... LINEAGE>
using AbstractPoly = typename detail::Configure<detail::AbstractPolyImpl, Value, LINEAGE...>::type;

}

#endif /* SRC_UTIL_POLYCONTAINER_ABSTRACTPOLY_H_ */
