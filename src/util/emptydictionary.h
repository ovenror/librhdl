/*
 * emptydictionary.h
 *
 *  Created on: Apr 19, 2025
 *      Author: js
 */

#ifndef SRC_UTIL_EMPTYDICTIONARY_H_
#define SRC_UTIL_EMPTYDICTIONARY_H_

#include "dictionary.h"

namespace rhdl {

template <class T>
class EmptyDictionary : public Dictionary<T> {
	using typename Dictionary<T>::CStrings;

public:
	static constexpr auto at_error = "Empty dictionary is empty.";

	const T & at(const std::string &name) const override
	{
		throw std::out_of_range(at_error);
	}

	const T & at(const char *name) const override {
		throw std::out_of_range(at_error);
	}

	bool contains(const std::string &name) const override {return false;}
	bool contains(const char *name) const override {return false;}

	size_t size() const override {return 0;}

	virtual const CStrings &c_strings() const {return c_strings_;}

private:
	std::vector<const char *> c_strings_ = {nullptr};
};

}



#endif /* SRC_UTIL_EMPTYDICTIONARY_H_ */
