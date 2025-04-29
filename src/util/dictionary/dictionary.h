/*
 * dictionary.h
 *
 *  Created on: Sep 29, 2024
 *      Author: ovenror
 */

#ifndef SRC_UTIL_DICTIONARY_DICTIONARY_H_
#define SRC_UTIL_DICTIONARY_DICTIONARY_H_

#include <util/dictionary/dictionarybase.h>
#include "util/util.h"
#include <string>

namespace rhdl::dictionary {

template <class T>
class Dictionary : public Dictionary<const T> {};

template <class T>
class Dictionary<const T> : public DictionaryBase
{
	static constexpr bool returnRef = !std::is_pointer_v<T>;

public:
	using StoreType = T;
	using ReturnType = std::conditional_t<
			std::is_pointer_v<T>, const std::remove_pointer_t<T> *, const T &>;

	virtual ~Dictionary() {};

	virtual ReturnType at(const std::string &name) const = 0;
	virtual ReturnType at(const char *name) const = 0;
};

}

#endif /* SRC_UTIL_DICTIONARY_DICTIONARY_H_ */
