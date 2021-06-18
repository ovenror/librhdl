/*
 * virtuals.h
 *
 *  Created on: Aug 22, 2021
 *      Author: ovenror
 */

#ifndef SRC_UTIL_POLYCONTAINER_VIRTUALS_H_
#define SRC_UTIL_POLYCONTAINER_VIRTUALS_H_

namespace rhdl {
namespace polycontainer {
namespace detail {

template <class C, class I, class... BASES>
class Virtuals : public BASES::Virtuals... {
	using I = typename C::iterator;

public:
	virtual ~Virtuals() {}

	virtual I begin(const C &) const = 0;
	virtual I end(const C &) const = 0;
	virtual I find(const typename I::value_type &) const = 0;
};

} /* namespace detail */
} /* namespace polycontainer */
} /* namespace rhdl */

#endif /* SRC_UTIL_POLYCONTAINER_VIRTUALS_H_ */
