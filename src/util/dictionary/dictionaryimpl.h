/*
 * dictionaryimpl.h
 *
 *  Created on: Apr 29, 2025
 *      Author: js
 */

#ifndef SRC_UTIL_DICTIONARY_DICTIONARYIMPL_H_
#define SRC_UTIL_DICTIONARY_DICTIONARYIMPL_H_

#include <util/dictionary/dictionaryimplbase.h>

namespace rhdl::dictionary {

template <class T, class ORDER, class LOOKUP = order::Lexical>
class DictionaryImpl : public DictionaryImplBase<
		detail::Constructed<T, Nature::LOOKUP, LOOKUP,
				detail::Constructed<T, Nature::ORDER, ORDER>>>
{
	using Order = detail::Constructed<T, Nature::ORDER, ORDER>;
	using Lookup = detail::Constructed<T, Nature::LOOKUP, LOOKUP, Order>;
	using Super = DictionaryImplBase<Lookup>;
	using typename Super::ReturnType;
	using typename Super::CStrings;
	using typename Super::LookupElement;
	using OrderPointer = typename Order::Element;

	template <class T2>
	struct MakeRTfromOP {
		static ReturnType fromOrderPointer(OrderPointer p) {return *p;}
	};

	template <class T2>
	struct MakeRTfromOP<T2*> {
		static ReturnType fromOrderPointer(OrderPointer p) {return p;}
	};

	template <class T2>
	struct MakeRTfromOP<std::unique_ptr<T2>> {
		static ReturnType fromOrderPointer(OrderPointer p)
		{
			return at(p -> name()).element();
		}
	};

	ReturnType fromOrderPointer(OrderPointer p) const
	{
		return MakeRTfromOP<T>::fromOrderPointer(p);
	}

public:
	using iterator = typename Order::iterator;
	using const_iterator = typename Order::const_iterator;

	DictionaryImpl() {}
	DictionaryImpl(std::vector<T> &&);
	DictionaryImpl(DictionaryImpl&&);
	virtual ~DictionaryImpl() {}

	using Super::at;
	ReturnType at(size_t i) const {return fromOrderPointer(order_.at(i));}
	ReturnType front() const {return fromOrderPointer(order_.front());}
	ReturnType back() const {return fromOrderPointer(order_.back());}

	const T &add(T element) override;
	const T &replace(T element) override;

	void clear() override;

	iterator begin() {return order_.begin();}
	iterator end() {return order_.end();}
	const_iterator begin() const {return order_.begin();}
	const_iterator end() const {return order_.end();}
	const_iterator cbegin() const {return order_.cbegin();}
	const_iterator cend() const {return order_.cend();}

private:
	void recalc() const override;
	void computeCStrings(CStrings &) const override;

	void order_add(const LookupElement &);
	void order_replace(const LookupElement &);

	Order order_;
};

template <class T, class ORDER>
class DictionaryImpl<T, ORDER, ORDER> : public DictionaryImplBase<
		detail::Constructed<T, Nature::LOOKUP_ORDER, ORDER>>
{
	using Super = DictionaryImplBase<
			detail::Constructed<T, Nature::LOOKUP_ORDER, ORDER>>;
	using typename Super::ReturnType;

public:
	DictionaryImpl() {}
	DictionaryImpl(std::vector<T> &&);
	DictionaryImpl(DictionaryImpl &&);
	virtual ~DictionaryImpl() {}

	const T &add(T element) override;
	const T &replace(T element) override;
};

template<class T, class ORDER, class LOOKUP>
inline DictionaryImpl<T, ORDER, LOOKUP>::DictionaryImpl(
		std::vector<T> &&v)
{
	Super::init(std::move(v));
}

template<class T, class ORDER, class LOOKUP>
inline DictionaryImpl<T, ORDER, LOOKUP>::DictionaryImpl(
		DictionaryImpl &&moved)
		: Super(std::move(moved)), order_(std::move(moved.order_)) {}

template<class T, class ORDER>
inline DictionaryImpl<T, ORDER, ORDER>::DictionaryImpl(std::vector<T> &&v)
{
	Super::init(std::move(v));
}

template<class T, class ORDER>
inline DictionaryImpl<T, ORDER, ORDER>::DictionaryImpl(DictionaryImpl&& moved)
		: Super(std::move(moved))
{
}

template<class T, class ORDER>
inline const T &DictionaryImpl<T, ORDER, ORDER>::add(T element)
{
	auto &result = Super::add(std::move(element));
	Super::recalc();
	return result;
}

template<class T, class ORDER, class LOOKUP>
inline const T &DictionaryImpl<T, ORDER, LOOKUP>::add(T element) {
	auto &le = Super::add(LookupElement(std::move(element)));

	auto order_result = order_.add(detail::orderPointer(le.element()));

	if (!order_result.has_value()) {
		recalc();
		return le.element();
	}

	auto [iter, index] = order_result.value();

	le.order_iterator_ = iter;
	le.order_index_ = index;

	if (Super::c_stringcache_.invalid())
		return le.element();

	assert (Super::size() == Super::c_strings_.size());

	if (index != Super::size() - 1) {
		Super::c_stringcache_.invalidate();
		return le.element();
	}

	Super::c_strings_.back() = le.c_str();
	Super::c_strings_.push_back(nullptr);

	const T &e = le.element();
	return e;
}

template<class T, class ORDER, class LOOKUP>
inline const T &DictionaryImpl<T, ORDER, LOOKUP>::replace(T element)
{
	const auto old_iter = Super::find(element -> name());
	auto old_order_index = old_iter -> order_index_;
	auto old_order_iter = old_iter -> order_iterator_;

	LookupElement new_le = LookupElement(std::move(element), old_order_index);

	const LookupElement &le = Super::replace(old_iter, std::move(new_le));

	auto order_result = order_.replace(
			old_order_iter, old_order_index, detail::orderPointer(le.element()));

	if (!order_result.has_value()) {
		recalc();
		return le.element();
	}

	auto [iter, index] = order_result.value();
	le.order_iterator_ = iter;

	if (le.order_index_ == index) {
		if (!Super::c_stringcache_.invalid())
			Super::c_strings_[index] = le.c_str();
	}
	else {
		Super::c_stringcache_.invalidate();
		le.order_index_ = index;
	}

	const T &e = le.element();
	return e;
}

template<class T, class ORDER>
inline const T& DictionaryImpl<T, ORDER, ORDER>::replace(T element)
{
	auto &result = Super::replace(std::move(element));
	Super::recalc();
	return result;
}

template<class T, class ORDER, class LOOKUP>
inline void DictionaryImpl<T, ORDER, LOOKUP>::clear()
{
	Super::clear();
	order_.clear();
}

template<class T, class ORDER, class LOOKUP>
inline void DictionaryImpl<T, ORDER, LOOKUP>::recalc() const
{
	Super::recalc();

	size_t index = 0;

	for (auto i = order_.begin(); i != order_.end(); ++i) {
		auto &le = Lookup::at((*i) -> name());
		le.order_iterator_ = i;
		le.order_index_ = index++;
	}
}

template<class T, class ORDER, class LOOKUP>
inline void DictionaryImpl<T, ORDER, LOOKUP>::computeCStrings(
		CStrings &cstrings) const
{
	cstrings.clear();

	for (const OrderPointer p : order_) {
		cstrings.push_back(p -> name().c_str());
	}

	cstrings.push_back(nullptr);
}

}

#endif /* SRC_UTIL_DICTIONARY_DICTIONARYIMPL_H_ */
