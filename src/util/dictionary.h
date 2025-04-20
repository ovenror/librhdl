/*
 * dictionary.h
 *
 *  Created on: Sep 29, 2024
 *      Author: ovenror
 */

#ifndef SRC_UTIL_DICTIONARY_H_
#define SRC_UTIL_DICTIONARY_H_

#include "dictionarybase.h"
#include "util/util.h"

namespace rhdl {

template <class T>
class Dictionary : public Dictionary<const T> {};

template <class T>
class Dictionary<const T> : public DictionaryBase
{
	static constexpr bool returnConst =
			!std::is_arithmetic_v<T> && !std::is_integral_v<T>;
	static constexpr bool returnRef = returnConst && !std::is_pointer_v<T>;

	using MaybeConst = cond_const<returnConst, T>;

public:
	using StoreType = T;
	using ReturnType = std::conditional_t<returnRef, MaybeConst&, MaybeConst>;

	virtual ~Dictionary() {};

	virtual ReturnType at(const std::string &name) const = 0;
	virtual ReturnType at(const char *name) const = 0;
};

}

#endif /* SRC_UTIL_DICTIONARY_H_ */
