/*
 * dictionaryimpl.h
 *
 *  Created on: Apr 29, 2025
 *      Author: js
 */

#ifndef SRC_UTIL_DICTIONARY_DICTIONARYIMPL_H_
#define SRC_UTIL_DICTIONARY_DICTIONARYIMPL_H_

#include "dictionaryimplbase.h"
#include "util/dereference_iterator.h"
#include <functional>
#include <boost/iterator/transform_iterator.hpp>

namespace rhdl::dictionary {

namespace detail {
template<class BASE, class T, class iterator> class IteratorImpl;
}

template <class T, class ORDER, class LOOKUP = order::Lexical>
class DictionaryImpl : public detail::DictionaryImplBase<
		detail::ConstructedLookup<T, LOOKUP,
				detail::ConstructedOrder<T, ORDER>>>
{
	using This = DictionaryImpl<T, ORDER, LOOKUP>;
	using Order = detail::ConstructedOrder<T, ORDER>;
	using OrderPointer = typename Order::Element;
	using Lookup = detail::ConstructedLookup<T, LOOKUP, Order>;
	using Super = detail::DictionaryImplBase<Lookup>;
	using GetElement = std::function<T &(OrderPointer)>;
	using GetConstElement = std::function<const T &(OrderPointer)>;
	using oiterator = typename Order::iterator;

protected:
	using typename Super::iterator_impl;
	using typename Super::const_iterator_impl;
	using IterImplPtr = typename Dictionary<T>::IterImplPtr;
	using ConstIterImplPtr = typename Dictionary<const T>::IterImplPtr;
	using typename Super::LookupElement;

public:
	using const_iterator = boost::transform_iterator<GetConstElement, oiterator>;
	using iterator = boost::transform_iterator<GetElement, oiterator>;
	using typename Super::CStrings;

private:
	using II = detail::IteratorImpl<iterator_impl, T, iterator>;
	using CII = detail::IteratorImpl<const_iterator_impl, const T, const_iterator>;

	template <class T2>
	struct MakeRTfromOP {
		static const T2& fromOrderPointer(const OrderPointer p, const This &dict) {return *p;}
		static T2& fromOrderPointer(const OrderPointer p, This &dict) {return *p;}
	};

	template <class T2>
	struct MakeRTfromOP<T2*> {
		static T2* const & fromOrderPointer(const OrderPointer &p, const This &dict) {return p;}
		static T2* & fromOrderPointer(OrderPointer &p, This &dict) {return p;}
	};

	template <class T2>
	struct MakeRTfromOP<T2* const> {
		static T2* const & fromOrderPointer(const OrderPointer &p, const This &dict) {return p;}
	};

	template <class T2>
	struct MakeRTfromOP<std::unique_ptr<T2>> {
		static const std::unique_ptr<T2>& fromOrderPointer(const OrderPointer p, const This &dict)
		{
			return dict.at(p -> name());
		}
		static std::unique_ptr<T2>& fromOrderPointer(const OrderPointer p, This &dict)
		{
			return dict.at(p -> name());
		}
	};

	template <class T2>
	struct MakeRTfromOP<const std::unique_ptr<T2>> {
		static const std::unique_ptr<T2>& fromOrderPointer(const OrderPointer p, const This &dict)
		{
			return dict.at(p -> name());
		}
	};

	const T &fromOrderPointer(OrderPointer p) const
	{
		return MakeRTfromOP<T>::fromOrderPointer(p, *this);
	}

	T &fromOrderPointer(OrderPointer p)
	{
		return MakeRTfromOP<T>::fromOrderPointer(p, *this);
	}

public:
	DictionaryImpl() {}
	DictionaryImpl(std::vector<T> &&);
	DictionaryImpl(DictionaryImpl&&);
	virtual ~DictionaryImpl() {}

	using Super::at;
	const T& at(size_t i) const {return fromOrderPointer(order_.at(i));}
	const T& front() const {return fromOrderPointer(order_.front());}
	const T& back() const {return fromOrderPointer(order_.back());}

	T& at(size_t i) {return fromOrderPointer(order_.at(i));}
	T& front() {return fromOrderPointer(order_.front());}
	T& back() {return fromOrderPointer(order_.back());}

	T& add(T &&element) override;
	T& replace(T &&element) override;
	T erase(const std::string &) override;
	T erase(const char *) override;
	T erase(iterator i) {return erase((*i) -> name());}

	void clear() override;

	iterator begin() {return elemIter(order_.cbegin());}
	iterator end() {return elemIter(order_.cend());}
	const_iterator begin() const {return elemIter(order_.cbegin());}
	const_iterator end() const {return elemIter(order_.cend());}
	const_iterator cbegin() const {return elemIter(order_.cbegin());}
	const_iterator cend() const {return elemIter(order_.cend());}

private:
	void recalc() const override;
	void computeCStrings(CStrings &) const override;

	const_iterator elemIter(oiterator i) const
	{
		return boost::make_transform_iterator(i, [&](const OrderPointer p) -> const T &{return fromOrderPointer(p);});
	}

	iterator elemIter(oiterator i)
	{
		return boost::make_transform_iterator(i, [&](const OrderPointer p) -> T &{return fromOrderPointer(p);});
	}

	IterImplPtr begin_impl();
	IterImplPtr end_impl();
	ConstIterImplPtr cbegin_impl() const override;
	ConstIterImplPtr cend_impl() const override;

protected:
	Order order_;
};

template <class T, class ORDER>
class DictionaryImpl<T, ORDER, ORDER> : public detail::DictionaryImplBase<
		detail::ConstructedLookup<T, ORDER>>
{
	using LookupAndOrder = detail::ConstructedLookup<T, ORDER>;
	using Super = detail::DictionaryImplBase<LookupAndOrder>;
	using IterImpl = typename Dictionary<T>::iterator_impl;
	using ConstIterImpl = typename Dictionary<const T>::iterator_impl;
	using IterImplPtr = typename Dictionary<T>::IterImplPtr;
	using ConstIterImplPtr = typename Dictionary<const T>::IterImplPtr;
	using literator = typename LookupAndOrder::const_iterator;
	using typename Super::LookupElement;
	using GetElement = std::function<T &(const LookupElement &)>;
	using GetConstElement = std::function<const T &(const LookupElement &)>;

public:
	using const_iterator = boost::transform_iterator<GetConstElement, literator>;
	using iterator = boost::transform_iterator<GetElement, literator>;

private:
	using II = detail::IteratorImpl<IterImpl, T, iterator>;
	using CII = detail::IteratorImpl<ConstIterImpl, const T, const_iterator>;

public:
	DictionaryImpl() {}
	DictionaryImpl(std::vector<T> &&);
	DictionaryImpl(DictionaryImpl &&);
	virtual ~DictionaryImpl() {}

	using Super::erase;
	T erase(iterator i) {std::move(Super::erase(i.base())).move_element();}

	iterator begin() {return elemIter(Super::begin());}
	iterator end() {return elemIter(Super::end());}
	const_iterator begin() const {return elemIter(Super::begin());}
	const_iterator end() const {return elemIter(Super::end());}
	const_iterator cbegin() const {return elemIter(Super::begin());}
	const_iterator cend() const {return elemIter(Super::end());}

private:
	iterator elemIter(literator i)
	{
		return boost::make_transform_iterator(i, [](const LookupElement &le) -> T &{return le.element();});
	}

	const_iterator elemIter(literator i) const
	{
		return boost::make_transform_iterator(i, [](const LookupElement &le) -> const T &{return le.element();});
	}

	IterImplPtr begin_impl();
	IterImplPtr end_impl();
	ConstIterImplPtr cbegin_impl() const override;
	ConstIterImplPtr cend_impl() const override;
};

template<class T, class ORDER, class LOOKUP>
inline DictionaryImpl<T, ORDER, LOOKUP>::DictionaryImpl(
		std::vector<T> &&v)
{
	Super::init(std::move(v));
}

template<class T, class ORDER>
inline DictionaryImpl<T, ORDER, ORDER>::DictionaryImpl(std::vector<T> &&v)
{
	Super::init(std::move(v));
}

template<class T, class ORDER, class LOOKUP>
inline DictionaryImpl<T, ORDER, LOOKUP>::DictionaryImpl(
		DictionaryImpl &&moved)
		: Super(std::move(moved)), order_(std::move(moved.order_)) {}

template<class T, class ORDER>
inline DictionaryImpl<T, ORDER, ORDER>::DictionaryImpl(DictionaryImpl &&moved)
		: Super(std::move(moved)) {}

template<class T, class ORDER, class LOOKUP>
inline T& DictionaryImpl<T, ORDER, LOOKUP>::add(T &&element) {
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

	T &e = le.element();
	return e;
}

template<class T, class ORDER, class LOOKUP>
inline T& DictionaryImpl<T, ORDER, LOOKUP>::replace(T &&element)
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

	T &e = le.element();
	return e;
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
		const auto &le = *Super::find((*i) -> name());
		le.order_iterator_ = i;
		le.order_index_ = index++;
	}
}

template<class T, class ORDER, class LOOKUP>
inline T DictionaryImpl<T, ORDER, LOOKUP>::erase(const std::string &name)
{
	LookupElement removed(Super::erase(Super::find(name)));
	auto index = removed.order_index_;
	auto iter = removed.order_iterator_;

	assert(*iter == order_.at(index));
	assert(detail::orderPointer(removed.element()) == order_.at(index));

	T result(std::move(removed).move_element());

	if (index != order_.size() - 1) {
		order_.erase(iter);
		recalc();
		return std::move(result);
	}

	order_.pop_back();

	if (Super::c_stringcache_.invalid())
		return std::move(result);

	Super::c_strings_.pop_back();
	Super::c_strings_.back() = nullptr;
	return std::move(result);
}

template<class T, class ORDER, class LOOKUP>
inline T DictionaryImpl<T, ORDER, LOOKUP>::erase(const char *name)
{
	return erase(std::string(name));
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

template<class T, class ORDER, class LOOKUP>
inline typename DictionaryImpl<T, ORDER, LOOKUP>::ConstIterImplPtr
		DictionaryImpl<T, ORDER, LOOKUP>::cbegin_impl() const
{
	return std::make_unique<CII>(cbegin());
}

template<class T, class ORDER, class LOOKUP>
inline typename DictionaryImpl<T, ORDER, LOOKUP>::ConstIterImplPtr
		DictionaryImpl<T, ORDER, LOOKUP>::cend_impl() const
{
	return std::make_unique<CII>(cend());
}

template<class T, class ORDER, class LOOKUP>
inline typename DictionaryImpl<T, ORDER, LOOKUP>::IterImplPtr
		DictionaryImpl<T, ORDER, LOOKUP>::begin_impl()
{
	return std::make_unique<II>(begin());
}

template<class T, class ORDER, class LOOKUP>
inline typename DictionaryImpl<T, ORDER, LOOKUP>::IterImplPtr
		DictionaryImpl<T, ORDER, LOOKUP>::end_impl()
{
	return std::make_unique<II>(end());
}

template<class T, class ORDER>
inline typename DictionaryImpl<T, ORDER, ORDER>::ConstIterImplPtr
		DictionaryImpl<T, ORDER, ORDER>::cbegin_impl() const
{
	return std::make_unique<CII>(cbegin());
}

template<class T, class ORDER>
inline typename DictionaryImpl<T, ORDER, ORDER>::ConstIterImplPtr
		DictionaryImpl<T, ORDER, ORDER>::cend_impl() const
{
	return std::make_unique<CII>(cend());
}

template<class T, class ORDER>
inline typename DictionaryImpl<T, ORDER, ORDER>::IterImplPtr
		DictionaryImpl<T, ORDER, ORDER>::begin_impl()
{
	return std::make_unique<II>(begin());
}

template<class T, class ORDER>
inline typename DictionaryImpl<T, ORDER, ORDER>::IterImplPtr
		DictionaryImpl<T, ORDER, ORDER>::end_impl()
{
	return std::make_unique<II>(end());
}

namespace detail {

template<class BASE, class T, class iterator>
class IteratorImpl : public BASE
{
public:
	IteratorImpl(iterator i) : i_(std::move(i)) {}

	IteratorImpl &operator++() override {++i_; return *this;}
	T &operator*() const override {return *i_;}
	//T *operator->() const override {return &static_cast<T>(*i_);}

	bool operator==(const BASE &other) const {return other.template equalsConcrete<IteratorImpl>(*this);}
	bool equals(const IteratorImpl &other) const {return i_ == other.i_;}

private:
	iterator i_;
};


}}

#endif /* SRC_UTIL_DICTIONARY_DICTIONARYIMPL_H_ */
