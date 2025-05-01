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
	using iterator = typename CONTAINER::iterator;
	using const_iterator = typename CONTAINER::const_iterator;

	Lookup() {}
	Lookup(Lookup &&moved) : CONTAINER(std::move(moved)) {}
	virtual ~Lookup() {}

	template <class STRING>
	bool contains(const STRING name) const;

	template <class STRING>
	const Element &at(const STRING name) const;

	const Element &at(size_t) const;
	const Element &front() const {return *CONTAINER::cbegin();}
	const Element &back() const {return *--CONTAINER::cend();}

	const Element &add(Element &&);
	const Element &replace(Element &&);
	const Element &replace(const_iterator, Element &&);

	template <class STRING>
	Element erase(STRING name);

	Element erase(const_iterator);

	template <class STRING>
	const_iterator find(const STRING name) const;

protected:
	const_iterator add_internal(Element &&);
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
inline const typename Lookup<CONTAINER>::Element &Lookup<CONTAINER>::
		at(size_t i) const
{
	auto iter = CONTAINER::cbegin();
	std::advance(iter, i);
	return *iter;
}

template<class CONTAINER>
inline const typename Lookup<CONTAINER>::Element& Lookup<CONTAINER>::add(
		Element &&e)
{
	return *add_internal(std::move(e));
}

template <class CONTAINER>
inline typename Lookup<CONTAINER>::const_iterator
		Lookup<CONTAINER>::add_internal(Element &&element)
{
	auto [i, success] = CONTAINER::insert(std::move(element));

	if (!success) {
		throw std::out_of_range("Lookup::add(): Element exists.");
	}

	return i;
}

template <class CONTAINER>
inline const typename Lookup<CONTAINER>::Element
		&Lookup<CONTAINER>::replace(Element &&element)
{
	auto i = find(element.name());
	return replace(i, std::move(element));
}

template<class CONTAINER>
inline const typename Lookup<CONTAINER>::Element &Lookup<CONTAINER>::replace(
		const_iterator i, Element &&e)
{
	assert(i -> name() == e.name());

	CONTAINER::erase(i);
	auto [iter, inserted] = CONTAINER::insert(std::move(e));
	assert (inserted);
	return *iter;
}

template<class CONTAINER>
template <class STRING>
inline typename CONTAINER::const_iterator Lookup<CONTAINER>::
		find(const STRING name) const
{
	auto i = CONTAINER::template find<const STRING>(name);

	if (i == CONTAINER::end()) {
		throw std::out_of_range("No such element.");
	}

	return i;
}

template<class CONTAINER>
inline typename Lookup<CONTAINER>::Element Lookup<CONTAINER>::erase(
		const_iterator i)
{
	auto nh = CONTAINER::extract(i);
	return std::move(nh.value());
}

template<class CONTAINER>
template<class STRING>
inline typename Lookup<CONTAINER>::Element Lookup<CONTAINER>::
		erase(STRING name)
{
	//return *std::make_move_iterator(find(name));

	/* somehow, std::make_move_iterator won't work */

	auto i = find(name);
	return erase(i);
}

}

#endif /* SRC_UTIL_DICTIONARY_LOOKUP_H_ */
