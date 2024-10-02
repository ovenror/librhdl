/*
 * dictionary.h
 *
 *  Created on: Sep 29, 2024
 *      Author: ovenror
 */

#ifndef SRC_UTIL_DICTIONARY_H_
#define SRC_UTIL_DICTIONARY_H_

#include <set>
#include <vector>

namespace rhdl {

template <class T>
class Dictionary {
public:
	Dictionary() {};
	virtual ~Dictionary() {};

	virtual T& at(const std::string &name) = 0;
	virtual const T& at(const std::string &name) const = 0;
	virtual const T& at(const char *name) const = 0;

	virtual std::size_t size() const = 0;

	virtual const std::vector<const char*> &c_strings() const = 0;
};

}

#endif /* SRC_UTIL_DICTIONARY_H_ */
