/*
 * lookup.h
 *
 *  Created on: Apr 26, 2025
 *      Author: js
 */

#ifndef SRC_UTIL_DICTIONARY_LOOKUP_H_
#define SRC_UTIL_DICTIONARY_LOOKUP_H_

#include <util/dictionary/dictionary.h>
#include <util/dictionary/dictionaryimpldefs.h>
#include <cassert>

namespace rhdl::dictionary {

template <class CONTAINER>
class Lookup : public CONTAINER {
public:
	using Element = typename CONTAINER::value_type;
	using const_iterator = typename CONTAINER::const_iterator;

	virtual ~Lookup() {}

	template <class STRING>
	bool contains(const STRING name) const;

	template <class STRING>
	const Element &at(const STRING name) const;

	const Element &add(Element);
	const Element &replace(Element);
	const Element &replace(const_iterator, Element);

protected:
	template <class STRING>
	const_iterator find(const STRING name) const;

	const_iterator add_internal(Element);
};


template <class CONTAINER>
template<class STRING>
inline bool Lookup<CONTAINER>::contains(
		const STRING name) const
{
	return CONTAINER::template find<const STRING>(name) != CONTAINER::end();
}

template<class CONTAINER>
template<class STRING>
inline const typename Lookup<CONTAINER>::Element& Lookup<CONTAINER>::at(
		const STRING name) const
{
	return *find(name);
}

template<class CONTAINER>
inline const typename Lookup<CONTAINER>::Element& Lookup<CONTAINER>::add(
		Element e)
{
	return *add_internal(std::move(e));
}


template <class CONTAINER>
inline typename Lookup<CONTAINER>::const_iterator
		Lookup<CONTAINER>::add_internal(Element element)
{
	auto [i, success] = CONTAINER::insert(std::move(element));

	if (!success) {
		throw std::out_of_range("Lookup::add(): Element exists.");
	}

	return i;
}

template <class CONTAINER>
inline const typename  Lookup<CONTAINER>::Element
		&Lookup<CONTAINER>::replace(Element element)
{
	auto i = find(element.name());
	return replace(i, std::move(element));
}

template<class CONTAINER>
inline const typename Lookup<CONTAINER>::Element &Lookup<CONTAINER>::replace(
		const_iterator i, Element e)
{
	assert(i -> name() == e.name());

	auto nh = CONTAINER::extract(i);
	auto &ref = nh.value();
	ref = std::move(e);
	auto [iter, inserted, node] = CONTAINER::insert(std::move(nh));
	assert (inserted);
	return *iter;
}

template<class CONTAINER>
template <class STRING>
inline typename Lookup<CONTAINER>::const_iterator Lookup<CONTAINER>::find(const STRING name) const
{
	auto i = CONTAINER::template find<const STRING>(name);

	if (i == CONTAINER::end()) {
		throw std::out_of_range("No such element.");
	}

	return i;
}

}

#endif /* SRC_UTIL_DICTIONARY_LOOKUP_H_ */
