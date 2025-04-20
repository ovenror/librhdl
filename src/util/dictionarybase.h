/*
 * dictionarybase.h
 *
 *  Created on: Apr 22, 2025
 *      Author: js
 */

#ifndef SRC_UTIL_DICTIONARYBASE_H_
#define SRC_UTIL_DICTIONARYBASE_H_

#include <stddef.h>
#include <string>
#include <vector>

namespace rhdl {

struct DictionaryBase {
	using CStrings = std::vector<const char *>;

	virtual ~DictionaryBase() {};

	virtual bool contains(const std::string &name) const = 0;
	virtual bool contains(const char *name) const = 0;

	virtual size_t size() const = 0;

	virtual const CStrings &c_strings() const = 0;

	virtual const DictionaryBase *underlying() const {return this;}

	virtual bool is_same_as(const DictionaryBase &other) const
	{
		return this == other.underlying();
	}
};

}


#endif /* SRC_UTIL_DICTIONARYBASE_H_ */
