/*
 * maker.h
 *
 *  Created on: Apr 28, 2025
 *      Author: js
 */

#ifndef SRC_UTIL_DICTIONARY_MAKER_H_
#define SRC_UTIL_DICTIONARY_MAKER_H_

#include "lookup.h"
#include "order.h"

namespace rhdl::dictionary::order {struct FCFS;}

namespace rhdl::dictionary::detail {

template <class T, bool LOOKUP, class CONSTRUCTED_ORDER = void>
struct MkElement {
	using type = LookupElement<T, CONSTRUCTED_ORDER>;
};

template <class T>
struct MkElement<T, false, void> {
	using type = OrderPointer<T>;
};

template <class T, bool LOOKUP, class CONSTRUCTED_ORDER>
using Element = typename MkElement<T, LOOKUP, CONSTRUCTED_ORDER>::type;

class Empty {};

template <class IMPL> static constexpr bool implOrder(typename IMPL::Order<Empty> *) {return true;}
template <class IMPL> static constexpr bool implOrder(...) {
	static_assert(!std::is_same_v<IMPL, order::FCFS>);
	return false;
}
template <class IMPL> static constexpr bool implLookup(typename IMPL::Lookup<Empty> *) {return true;}
template <class IMPL> static constexpr bool implLookup(...) {return false;}

template <class ORDER, bool LOOKUP, bool OWN_IMPL>
struct Implementation_;

template <class ORDER>
struct Implementation_<ORDER, true, false> {
	template <class Container>
	using type = Lookup<Container>;
};

template <class ORDER>
struct Implementation_<ORDER, true, true> {
	template <class Container>
	using type = typename ORDER::Lookup<Container>;
};

template <class ORDER>
struct Implementation_<ORDER, false, false> {
	template <class Container>
	using type = Order<Container>;
};

template <class ORDER>
struct Implementation_<ORDER, false, true> {
	template <class Container>
	using type = typename ORDER::Order<Container>;
};

template <class ORDER, bool LOOKUP>
struct Implementation {
	template <class Container>
	using type = typename Implementation_<
			ORDER, false,
			implOrder<ORDER>(nullptr)>::type<Container>;
};

template <class ORDER>
struct Implementation<ORDER, true> {
	template <class Container>
	using type = typename Implementation_<
			ORDER, true,
			implLookup<ORDER>(nullptr)>::type<Container>;
};

template <class ORDER> constexpr bool isLookupHelper(typename ORDER::IsLookup *) {return true;}
template <class ORDER> constexpr bool isLookupHelper(...) {
	static_assert(std::is_same_v<ORDER, order::FCFS>);
	return false;
}
template <class ORDER> constexpr bool isLookup = isLookupHelper<ORDER>(nullptr);

template <class T, class ORDER, bool LOOKUP, class CONSTRUCTED_ORDER = void>
struct Make
{
	static_assert(!std::is_same_v<ORDER, order::FCFS> || !LOOKUP);

	static_assert(
			!LOOKUP || isLookup<ORDER>,
			"Given order cannot be used as lookup.");

	static_assert(
			LOOKUP || std::is_same_v<CONSTRUCTED_ORDER, void>,
			"Only an order constructed as lookup needs constructed order.");

	using Elem = Element<T, LOOKUP, CONSTRUCTED_ORDER>;
	using Container = typename ORDER::Container<Elem>;

	using Impl = Implementation<ORDER, LOOKUP>;
	using type = typename Impl::type<Container>;
};

template <class T, class ORDER, class CONSTRUCTED_ORDER = void>
using ConstructedLookup = typename Make<T, ORDER, true, CONSTRUCTED_ORDER>::type;

template <class T, class ORDER>
using ConstructedOrder = typename Make<T, ORDER, false, void>::type;

}




#endif /* SRC_UTIL_DICTIONARY_MAKER_H_ */
