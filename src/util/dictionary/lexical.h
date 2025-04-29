/*
 * lexical.h
 *
 *  Created on: Apr 25, 2025
 *      Author: js
 */

#ifndef SRC_UTIL_DICTIONARY_LEXICAL_H_
#define SRC_UTIL_DICTIONARY_LEXICAL_H_

#include <util/dictionary/lookupwithorder.h>
#include <util/dictionary/nature.h>
#include <util/dictionary/order.h>
#include <set>

namespace rhdl::dictionary::order {

class Lexical {
	template <class> class Less;

public:
	static constexpr Nature nature = Nature::LOOKUP_ORDER;

	template <class Element>
	using Container = std::set<Element, Less<Element>>;
};

template <class Element>
class Lexical::Less {
	using Comparee = std::conditional_t<
			std::is_pointer_v<Element>, Element, const Element &>;

	template <class ArgT>
	using enable = std::enable_if_t<
			!std::is_same_v<const ArgT &, Comparee>, bool>;

public:
	using is_transparent = void;

	bool operator()(Comparee lhs, Comparee rhs) const {
		return lhs < rhs;
	}

	template <class RHS, enable<RHS> dummy = true>
	bool operator()(Comparee lhs, RHS rhs) const {
		return lhs < rhs;
	}

	template <class LHS, enable<LHS> dummy = true>
	bool operator()(LHS lhs, Comparee rhs) const {
		return rhs > lhs;
	}
};

}

#endif /* SRC_UTIL_DICTIONARY_LEXICAL_H_ */
