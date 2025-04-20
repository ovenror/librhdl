/*
 * dictionary.h
 *
 *  Created on: Sep 29, 2024
 *      Author: ovenror
 */

#ifndef SRC_UTIL_DICTIONARY_H_
#define SRC_UTIL_DICTIONARY_H_

#include <set>
#include <string>
#include <vector>

namespace rhdl {

template <class T>
class Dictionary {
public:
	using CStrings = std::vector<const char *>;

	Dictionary() {};
	virtual ~Dictionary() {};

	virtual bool contains(const std::string &name) const = 0;
	virtual bool contains(const char *name) const = 0;

	virtual const T &at(const std::string &name) const = 0;
	virtual const T &at(const char *name) const = 0;

	virtual size_t size() const = 0;

	virtual const CStrings &c_strings() const = 0;
};

}

#endif /* SRC_UTIL_DICTIONARY_H_ */
