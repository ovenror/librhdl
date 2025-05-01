/*
 * order.h
 *
 *  Created on: Apr 26, 2025
 *      Author: js
 */

#ifndef SRC_UTIL_DICTIONARY_ORDER_H_
#define SRC_UTIL_DICTIONARY_ORDER_H_

#include <stddef.h>
#include <optional>
#include <tuple>

namespace rhdl::dictionary {

template <class CONTAINER>
class Order : public CONTAINER {
	using Super = CONTAINER;

public:
	using const_iterator = typename CONTAINER::const_iterator;
	using iterator = const_iterator;

protected:
	using Result = std::optional<std::pair<const_iterator, size_t>>;

public:
	using Element = typename CONTAINER::value_type;

	virtual ~Order() {}

	Element at(size_t) const;
	Element front() const {return *Super::cbegin();}
	Element back() const {return *--Super::cend();}

	Result add(Element);
	Result replace(const_iterator, size_t, Element);
	void pop_back() {Super::erase(--Super::cend());}

	void clear() {return Super::clear();}

protected:
	size_t getIndex(const_iterator i) const;
};

template<class CONTAINER>
inline typename Order<CONTAINER>::Result Order<CONTAINER>::add(
		Element e)
{
	auto [i, success] = Super::insert(e);

	if (!success) {
		throw std::out_of_range("Order::add(): Element exists.");
	}

	return std::nullopt;
}

template<class CONTAINER>
inline typename Order<CONTAINER>::Result Order<CONTAINER>::replace(
		const_iterator i, size_t index, Element e)
{
	assert((*i) -> name() == e -> name());

	auto nh = Super::extract(i);
	Element old = nh.value();
	old = std::move(e);
	Super::insert(std::move(nh));

	return std::nullopt;
}

template<class CONTAINER>
inline typename Order<CONTAINER>::Element Order<CONTAINER>::at(size_t i) const
{
	auto iter = Super::begin();
	std::advance(iter, i);
	return *iter;
}

template<class CONTAINER>
inline size_t Order<CONTAINER>::getIndex(const_iterator i) const
{
	return std::distance(Super::begin(), i);
}

}


#endif /* SRC_UTIL_DICTIONARY_ORDER_H_ */
