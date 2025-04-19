/*
 * mutabledictionary.h
 *
 *  Created on: Apr 18, 2025
 *      Author: js
 */

#ifndef SRC_UTIL_EXTENSIBLEDICTIONARY_H_
#define SRC_UTIL_EXTENSIBLEDICTIONARY_H_

#include "dictionary.h"

namespace rhdl {

template <class T>
class ExtensibleDictionary : public Dictionary<T> {
	using Super = Dictionary<T>;

public:
	using typename Super::CStrings;

	ExtensibleDictionary() {}
	virtual ~ExtensibleDictionary() {}

	virtual const T &add(T element) = 0;
};

} /* namespace rhdl */

#endif /* SRC_UTIL_EXTENSIBLEDICTIONARY_H_ */
