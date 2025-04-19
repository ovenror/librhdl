/*
 * mutabledictionary.h
 *
 *  Created on: Apr 18, 2025
 *      Author: js
 */

#ifndef SRC_UTIL_MUTABLEDICTIONARY_H_
#define SRC_UTIL_MUTABLEDICTIONARY_H_

#include "dictionary.h"

namespace rhdl {

template <class T>
class MutableDictionary : public Dictionary<T> {
	using Super = Dictionary<T>;

public:
	using typename Super::CStrings;

	MutableDictionary() {}
	virtual ~MutableDictionary() {}

	virtual const T &add(T element) = 0;
	virtual const T &replace(T element) = 0;

	virtual void clear() = 0;
};

} /* namespace rhdl */

#endif /* SRC_UTIL_MUTABLEDICTIONARY_H_ */
