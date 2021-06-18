/*
 * tpolyiterator.h
 *
 *  Created on: Aug 22, 2021
 *      Author: ovenror
 */

#ifndef SRC_UTIL_POLYCONTAINER_POLYITERATOR_H_
#define SRC_UTIL_POLYCONTAINER_POLYITERATOR_H_

#include <memory>

namespace rhdl::polycontainer::detail {

template <class V, class... BASES>
struct PolyIterator : public BASES::PolyIterator... {
	using value_type = V;

	bool operator==(const PolyIterator &i) const {return &**this == &*i;}
	bool operator!=(const PolyIterator &i) const {return !(*this == i);}

	virtual PolyIterator& operator++() = 0;
	virtual V *operator->() const = 0;
	virtual V &operator*() const = 0;

	virtual ~PolyIterator() {}

	virtual PolyIterator *clone() const = 0;
};

} /* namespace rhdl::polycontainer::detail */

#endif /* SRC_UTIL_POLYCONTAINER_POLYITERATOR_H_ */
