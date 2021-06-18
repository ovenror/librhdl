/*
 * transparentless.h
 *
 *  Created on: Jul 9, 2021
 *      Author: ovenror
 */

#ifndef SRC_UTIL_TRANSPARENTLESS_H_
#define SRC_UTIL_TRANSPARENTLESS_H_

#include "list.h"
#include "util.h"

namespace rhdl {

template <class KeyMaker>
class TransparentLess;

template <class Primary, class Transparent, class Index, class Base = None>
struct UnifiedKeys;

template <class Primary, class Transparent, class Index, class Base>
class TransparentLess<UnifiedKeys<Primary, Transparent, Index, Base>> {
	static_assert(true, "Template parameter 'KeyMaker' must be a class "
			"that derives from UnifiedKeys<>, not UnifiedKeys itself.");
};

template <class, class> class TransparentLessBase;

template <class KeyMaker>
class TransparentLess<KeyMaker> : public TransparentLessBase<KeyMaker, typename KeyMaker::Transparent> {
	using Primary = typename KeyMaker::Primary;
public:
	constexpr bool operator()(const Primary &lhs, const Primary &rhs)
	{
		return lhs < rhs;
	}
};

class TransparentLessComparator<>

template <class KeyMaker, class AuxKey, class AuxKeys...>
class TransparentLessBase<KeyMaker, List<AuxKey, AuxKeys...>> :
	public TransparentLessBase<KeyMaker, List<AuxKeys...>>

{

};

#if 0
template <class KeyMaker, class AuxKey, class... AuxKeys>
class TransparentLessBase<KeyMaker, AuxKey, AuxKeys...> : public TransparentLessBase<KeyMaker, AuxKeys...> {
	constexpr bool operator()(const AuxKey &lhs, const AuxKey &rhs)
	{
		return *this(lhs) < *this(rhs);
	}

	constexpr bool operator()(const AuxKey &lhs, const Key &rhs)
	{
		return *this(lhs) < rhs;
	}

	constexpr bool operator()(const Key &lhs, const AuxKey &rhs)
	{
		return lhs < *this(rhs);
	}
};

template <class KeyMaker>
class TransparentLessBase<KeyMaker> : public KeyMaker {};


template <class KeyMaker, class AuxKey
#endif


template <class Primary, class Transparent, class Index, class Base>
struct UnifiedKeys {
	constexpr void error() {
		static_assert("Template parameters 'Transparent' and 'Index' have to "
				"be a List<...>, Base has to be a Child of UnifiedKeys");
	}
};

template <class, class, class...> class UnifiedKeysBase;

template <class Primary, class T1, class T2, class... TN, class... Index, class Base>
struct UnifiedKeys<Primary, List<T1, T2, TN...>, List<Index...>, Base> {
	constexpr void error() {
		static_assert("Not implemented for Transparent = List<> with more "
				"than 1 element.");
	}
};

template <class Primary, class... Transparent, class... Index>
struct UnifiedKeys<Primary, List<Transparent...>, List<Index...>, None> : public UnifiedKeysBase<None, Primary, Transparent..., Index...> {
	using Primary = Primary;
	using Transparent = List<Transparent>;
	using Index = List<Index>;
};

template <class Primary, class... Transparent, class... Index, class Base>
struct UnifiedKeys<Primary, List<Transparent...>, List<Index...>, Base> : public UnifiedKeysBase<Base, Primary, Transparent..., Index...> {
	using Primary = Concat<typename Base::Primary, Primary>;
	using Transparent = Concat<typename Base::Transparent, Transparent>;
	using Index = Concat<typename Base::Index, Index>;
};

template <class Base, class Primary, class Other, class... Others>
class UnifiedKeysBase<Base, Primary, Other, Others...> : public UnifiedKeysBase<Base, Primary, Others...> {
	virtual Primary operator()(const Other &) = 0;
};

template <class Base, class Primary>
class UnifiedKeysBase<Base, Primary> : public Base {};

template <class Primary>
class UnifiedKeysBase<None, Primary> {};

} /* namespace rhdl */

#endif /* SRC_UTIL_TRANSPARENTLESS_H_ */
