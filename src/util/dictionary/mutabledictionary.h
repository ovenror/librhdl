/*
 * mutabledictionary.h
 *
 *  Created on: Apr 18, 2025
 *      Author: js
 */

#ifndef SRC_UTIL_DICTIONARY_MUTABLEDICTIONARY_H_
#define SRC_UTIL_DICTIONARY_MUTABLEDICTIONARY_H_

#include "dictionary.h"

namespace rhdl::dictionary {
namespace detail {

template<typename C, typename R, typename... A>
R ret(R(C::*)(A...) const);

template <class T> static constexpr bool hasNameHelper(decltype(&remove_any_pointer_t<T>::name)) {return true;}
template <class T> static constexpr bool hasNameHelper(...) {return false;}
template <class T> constexpr bool hasName = hasNameHelper<T>(nullptr);

template <class T, bool HASNAME>
struct CheckName {
	static constexpr bool ok = false;
};

template <class T>
struct CheckName<T, true> {
	static constexpr bool ok = std::is_same_v<decltype(ret(&remove_any_pointer_t<T>::name)), const std::string &>;
};

template <class T> constexpr bool nameOK = CheckName<T, hasName<T>>::ok;
}

template <class T>
class MutableDictionary : public Dictionary<T> {
	using Super = Dictionary<T>;

	static_assert(detail::hasName<T>);
	static_assert(detail::nameOK<T>);

public:
	using typename Super::CStrings;

	MutableDictionary() {}
	virtual ~MutableDictionary() {}

	virtual const T &add(T element) = 0;
	virtual const T &replace(T element) = 0;

	virtual void clear() = 0;
};

} /* namespace rhdl::dictionary */

#endif /* SRC_UTIL_DICTIONARY_MUTABLEDICTIONARY_H_ */
