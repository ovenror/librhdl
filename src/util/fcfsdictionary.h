/*
 * fcfsdictionary.h
 *
 *  Created on: Apr 18, 2025
 *      Author: js
 */

#ifndef SRC_UTIL_FCFSDICTIONARY_H_
#define SRC_UTIL_FCFSDICTIONARY_H_

#include <util/any_pointer.h>
#include <util/dictionaryimplbase.h>

namespace rhdl {

template <class T>
class FCFSDictionary : public DictionaryImplBase<T> {
	using Super = DictionaryImplBase<T>;

public:
	using typename Super::ReturnType;
	using SecondaryPointer = const typename remove_any_pointer<T>::type *;
	using typename Super::SetElement;
	using typename Super::CStrings;
	using Order = std::vector<SecondaryPointer>;
	using const_iterator = typename Order::const_iterator;
	using size_type = typename Order::size_type;

	FCFSDictionary();
	FCFSDictionary(std::vector<T>);

	FCFSDictionary(FCFSDictionary &&) = default;

	virtual ~FCFSDictionary() {}

	ReturnType front() const {return at(static_cast<size_type>(0));}
	ReturnType at(size_type index) const;
	using Super::at;

	using Super::add;
	void clear();

	const_iterator begin() const {return order_.begin();}
	const_iterator end() const {return order_.end();}
	const_iterator cbegin() const {return order_.begin();}
	const_iterator cend() const {return order_.end();}

private:
	const SetElement &add(SetElement element) override;

	template <class ArgT>
	SecondaryPointer secondaryPointer(ArgT &element) {return &*element;}

	std::vector<SecondaryPointer> order_;
};

template<class T>
inline FCFSDictionary<T>::FCFSDictionary(std::vector<T> init)
	: FCFSDictionary()
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
inline typename FCFSDictionary<T>::ReturnType FCFSDictionary<T>::at(size_type index) const
{
	return order_.at(index);
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
