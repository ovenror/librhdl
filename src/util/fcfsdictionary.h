/*
 * fcfsdictionary.h
 *
 *  Created on: Apr 18, 2025
 *      Author: js
 */

#ifndef SRC_UTIL_FCFSDICTIONARY_H_
#define SRC_UTIL_FCFSDICTIONARY_H_

#include "dictionarybase.h"
#include "util/remove_any_pointer.h"

namespace rhdl {

template <class T>
class FCFSDictionary : public DictionaryBase<T> {
	using Super = DictionaryBase<T>;

public:
	using SecondaryPointer = const typename remove_any_pointer<T>::type *;
	using typename Super::SetElement;
	using typename Super::CStrings;
	using Order = std::vector<SecondaryPointer>;
	using iterator = typename Order::iterator;

	FCFSDictionary();
	FCFSDictionary(std::vector<T>);

	FCFSDictionary(FCFSDictionary &&) = default;

	virtual ~FCFSDictionary() {}

	using Super::add;
	void clear();

	iterator begin() {return order_.begin();}
	iterator end() {return order_.end();}
	iterator begin() const {return order_.begin();}
	iterator end() const {return order_.end();}

private:
	const SetElement &add(SetElement element) override;

	template <class ArgT>
	SecondaryPointer secondaryPointer(ArgT &element) {return &*element;}

	std::vector<SecondaryPointer> order_;
};

template<class T>
inline FCFSDictionary<T>::FCFSDictionary(std::vector<T> init)
{
	for (T element : std::move(init)) {
		add(std::move(element));
	}
}

template<class T>
inline FCFSDictionary<T>::FCFSDictionary()
{
	Super::c_strings().push_back(nullptr);
}

template<class T>
inline void FCFSDictionary<T>::clear()
{
	Super::c_strings().clear();
	Super::c_strings().push_back(nullptr);
	clear();
}

template<class T>
const typename FCFSDictionary<T>::SetElement &FCFSDictionary<T>::add(SetElement element)
{
	order_.push_back(secondaryPointer(element.element()));
	element.index() = Super::size();
	Super::c_strings().back() = element.c_str();
	Super::c_strings().push_back(nullptr);
	return Super::add(std::move(element));
}

} /* namespace rhdl */

#endif /* SRC_UTIL_FCFSDICTIONARY_H_ */
