/*
 * dereference_iterator.h
 *
 *  Created on: Jun 16, 2021
 *      Author: ovenror
 */

#ifndef SRC_UTIL_DEREFERENCE_ITERATOR_H_
#define SRC_UTIL_DEREFERENCE_ITERATOR_H_

#include "util.h"
#include <boost/iterator/transform_iterator.hpp>
#include <utility>

namespace rhdl {

namespace dereferencing_iterator {
template <class iterator, bool CONST>
class Functor {
	using Argument = decltype(*std::declval<iterator>());
	using NonconstResult = decltype(**std::declval<iterator>());
	using Result = typename cond_const<CONST, NonconstResult>::type;

public:
	Result operator()(Argument a) const {return *a;}
};
}

template <class iterator, bool CONST = false>
auto deref_iterator(iterator i)
{
	return boost::make_transform_iterator(
			i, dereferencing_iterator::Functor<iterator, CONST>());
}

template <class iterator>
auto const_deref_iterator(iterator i)
{
	return deref_iterator<iterator, true>(i);
}

template <class iterator, bool CONST = false>
using DereferencingIterator = decltype(
		deref_iterator<iterator, CONST>(std::declval<iterator>()));

template <class iterator>
using ConstDereferencingIterator = DereferencingIterator<iterator, true>;

}

#endif /* SRC_UTIL_DEREFERENCE_ITERATOR_H_ */
