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
class Order {
public:
	using iterator = typename CONTAINER::iterator;
	using const_iterator = typename CONTAINER::const_iterator;

protected:
	using Result = std::optional<std::pair<const_iterator, size_t>>;

public:
	using Element = typename CONTAINER::value_type;

	virtual ~Order() {}

	Element at(size_t);
	const Element at(size_t) const;

	Element front() {return *begin();}
	const Element front() const {return *begin();}

	Element back() {return *--end();}
	const Element back() const {return *--end();}

	Result add(Element);
	Result replace(const_iterator, size_t, Element);

	void clear() {return container_.clear();}

	iterator begin() {return container_.begin();}
	iterator end() {return container_.end();}
	const_iterator begin() const {return container_.begin();}
	const_iterator end() const {return container_.end();}
	const_iterator cbegin() const {return container_.cbegin();}
	const_iterator cend() const {return container_.cend();}

protected:
	size_t getIndex(const_iterator i) const;

	CONTAINER container_;
};

template<class CONTAINER>
inline typename Order<CONTAINER>::Result Order<CONTAINER>::add(
		Element e)
{
	auto [i, success] = container_.insert(e);

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

	auto nh = container_.extract(i);
	Element old = nh.value();
	old = std::move(e);
	container_.insert(std::move(nh));

	return std::nullopt;
}

template<class CONTAINER>
inline typename Order<CONTAINER>::Element Order<CONTAINER>::at(size_t i)
{
	auto iter = begin();
	std::advance(iter, i);
	return *iter;
}

template<class CONTAINER>
inline const typename Order<CONTAINER>::Element Order<CONTAINER>::at(size_t i) const
{
	auto iter = begin();
	std::advance(iter, i);
	return *iter;
}

template<class CONTAINER>
inline size_t Order<CONTAINER>::getIndex(const_iterator i) const
{
	return std::distance(begin(), i);
}

}


#endif /* SRC_UTIL_DICTIONARY_ORDER_H_ */
