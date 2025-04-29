/*
 * order.h
 *
 *  Created on: Apr 26, 2025
 *      Author: js
 */

#ifndef SRC_UTIL_DICTIONARY_LOOKUPWITHORDER_H_
#define SRC_UTIL_DICTIONARY_LOOKUPWITHORDER_H_

#include <util/dictionary/lookup.h>

namespace rhdl::dictionary {

template <class CONTAINER>
class LookupWithOrder : public Lookup<CONTAINER> {
	using Super = Lookup<CONTAINER>;

public:
	using Element = typename Super::Element;

private:
	using OrderPointer = detail::OrderPointer<Element>;

public:
	using iterator = typename Super::iterator;
	using const_iterator = typename Super::const_iterator;

	virtual ~LookupWithOrder() {}

	using Super::at;
	Element at(size_t);
	const Element at(size_t) const;

	Element front() {return *Super::begin();}
	const Element front() const {return *Super::begin();}

	Element back() {return *--Super::end();}
	const Element back() const {return *--Super::end();}
};

template<class CONTAINER>
inline typename LookupWithOrder<CONTAINER>::Element LookupWithOrder<CONTAINER>::at(size_t i)
{
	auto iter = Super::begin();
	std::advance(iter, i);
	return *iter;
}

}

#endif /* SRC_UTIL_DICTIONARY_LOOKUPWITHORDER_H_ */
