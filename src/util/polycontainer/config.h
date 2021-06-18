/*
 * getlineagebase.h
 *
 *  Created on: Aug 22, 2021
 *      Author: ovenror
 */

#ifndef SRC_UTIL_POLYCONTAINER_CONFIG_H_
#define SRC_UTIL_POLYCONTAINER_CONFIG_H_

namespace rhdl {
namespace polycontainer {
namespace detail {

template <class VALUE, class... BASES> struct Config;

template <template <class> class C, class Value, class... Lineage>
struct Configure {
	using type = C<Config<Value>>;
};

template <template <class> class C, class Value, class Parent, class... Lineage>
struct Configure<C, Value, Parent, Lineage...> {
	using type = C<Config<
			Value,
			typename Configure<C, Value>::type,
			typename Configure<C, Parent, Lineage...>::type>>;
};



} /* namespace detail */
} /* namespace polycontainer */
} /* namespace rhdl */

#endif /* SRC_UTIL_POLYCONTAINER_CONFIG_H_ */
