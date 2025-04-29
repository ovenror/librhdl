/*
 * maker.h
 *
 *  Created on: Apr 28, 2025
 *      Author: js
 */

#ifndef SRC_UTIL_DICTIONARY_MAKER_H_
#define SRC_UTIL_DICTIONARY_MAKER_H_

#include <util/dictionary/lookup.h>
#include <util/dictionary/lookupwithorder.h>
#include <util/dictionary/nature.h>
#include <util/dictionary/order.h>

namespace rhdl::dictionary::order {struct FCFS;}

namespace rhdl::dictionary::detail {

template <class T, Nature N, class CONSTRUCTED_ORDER = void>
struct MkElement {
	using type = LookupElement<T>;
};

template <class T>
struct MkElement<T, Nature::ORDER, void> {
	using type = OrderPointer<T>;
};

template <class T, class CONSTRUCTED_ORDER>
struct MkElement<T, Nature::LOOKUP, CONSTRUCTED_ORDER> {
	using type = LookupElement<T, CONSTRUCTED_ORDER>;
};

template <class T, Nature N, class CONSTRUCTED_ORDER>
using Element = typename MkElement<T, N, CONSTRUCTED_ORDER>::type;

class Empty {};

template <class IMPL> static constexpr bool implOrder(typename IMPL::Order<Empty> *) {return true;}
template <class IMPL> static constexpr bool implOrder(...) {
	static_assert(!std::is_same_v<IMPL, order::FCFS>);
	return false;
}
template <class IMPL> static constexpr bool implLookup(typename IMPL::Lookup<Empty> *) {return true;}
template <class IMPL> static constexpr bool implLookup(...) {return false;}
template <class IMPL> static constexpr bool implLookupWithOrder(typename IMPL::LookupWithOrder<Empty> *) {return true;}
template <class IMPL> static constexpr bool implLookupWithOrder(...) {return false;}

template <class ORDER, Nature N, bool OWN_IMPL>
struct Implementation_;

template <class ORDER>
struct Implementation_<ORDER, Nature::LOOKUP_ORDER, false> {
	template <class Container>
	using type = LookupWithOrder<Container>;
};

template <class ORDER>
struct Implementation_<ORDER, Nature::LOOKUP_ORDER, true> {
	template <class Container>
	using type = typename ORDER::LookupWithOrder<Container>;
};

template <class ORDER>
struct Implementation_<ORDER, Nature::LOOKUP, false> {
	template <class Container>
	using type = Lookup<Container>;
};

template <class ORDER>
struct Implementation_<ORDER, Nature::LOOKUP, true> {
	template <class Container>
	using type = typename ORDER::Lookup<Container>;
};

template <class ORDER>
struct Implementation_<ORDER, Nature::ORDER, false> {
	template <class Container>
	using type = Order<Container>;
};

template <class ORDER>
struct Implementation_<ORDER, Nature::ORDER, true> {
	template <class Container>
	using type = typename ORDER::Order<Container>;
};

template <class ORDER, Nature N>
struct Implementation {
	template <class Container>
	using type = typename Implementation_<
			ORDER, Nature::LOOKUP_ORDER,
			implLookupWithOrder<ORDER>(nullptr)>::type<Container>;
};

template <class ORDER>
struct Implementation<ORDER, Nature::ORDER> {
	template <class Container>
	using type = typename Implementation_<
			ORDER, Nature::ORDER,
			implOrder<ORDER>(nullptr)>::type<Container>;
};

template <class ORDER>
struct Implementation<ORDER, Nature::LOOKUP> {
	template <class Container>
	using type = typename Implementation_<
			ORDER, Nature::LOOKUP,
			implLookup<ORDER>(nullptr)>::type<Container>;
};

template <class T, Nature N, class ORDER, class CONSTRUCTED_ORDER = void>
struct Make
{
	static_assert(
			N == ORDER::nature || ORDER::nature == Nature::LOOKUP_ORDER,
			"Given order does not implement the given nature");

	using Impl = Implementation<ORDER, N>;
	using Container = typename ORDER::Container<Element<T, N, CONSTRUCTED_ORDER>>;
	using type = typename Impl::type<Container>;
};

template <class T, Nature N, class ORDER, class CONSTRUCTED_ORDER = void>
using Constructed = typename Make<T, N, ORDER, CONSTRUCTED_ORDER>::type;

}




#endif /* SRC_UTIL_DICTIONARY_MAKER_H_ */
