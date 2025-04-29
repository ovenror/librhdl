/*
 * fcfs.h
 *
 *  Created on: Apr 26, 2025
 *      Author: js
 */

#ifndef SRC_UTIL_DICTIONARY_FCFS_H_
#define SRC_UTIL_DICTIONARY_FCFS_H_

#include <util/dictionary/dictionaryimpldefs.h>
#include <util/dictionary/order.h>
#include <vector>

namespace rhdl::dictionary::order {

struct FCFS {
	static constexpr Nature nature = Nature::ORDER;

	template <class Element>
	using Container = std::vector<Element>;

	template <class Container>
	class Order;
};

template <class Container>
class FCFS::Order : public Container {
	using Super = Container;

public:
	using Element = typename Container::value_type;
	using typename Super::const_iterator;

protected:
	using Result = std::optional<std::pair<const_iterator, size_t>>;

public:
	virtual ~Order() {}

	Result add(Element);
	Result replace(const_iterator, size_t, Element);
};

template<class Container>
inline typename FCFS::Order<Container>::Result
		FCFS::Order<Container>::add(Element e)
{
	auto capa = Super::capacity();
	Super::push_back(std::move(e));

	if (capa == Super::capacity())
		return std::make_pair(Super::cend() - 1, Super::size() - 1);
	else
		return std::nullopt;
}

template<class Container>
inline typename FCFS::Order<Container>::Result
		FCFS::Order<Container>::replace(
				const_iterator i, size_t index, Element e)
{
	assert(index < Super::size());

	(*this)[index] = e;
	return std::make_pair(i, index);
}

}


#endif /* SRC_UTIL_DICTIONARY_FCFS_H_ */
