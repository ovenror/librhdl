/*
 * dictionary.h
 *
 *  Created on: May 12, 2021
 *      Author: ovenror
 */

#ifndef UTIL_LEXICALDICTIONARY_H_
#define UTIL_LEXICALDICTIONARY_H_

#include "util/cached.h"
#include <rhdl/construction/constructionexception.h>
#include <util/dictionary/dictionaryadapter.h>
#include <util/dictionary/lexical.h>
#include <util/dictionary/maker.h>
#include <util/dictionary/mutabledictionary.h>
#include <util/dictionary/unordered.h>
#include <unordered_set>
#include <tuple>
#include <vector>
#include <cassert>
#include <optional>

namespace rhdl::dictionary::detail {

template <class LOOKUP>
class DictionaryImplBase : public MutableDictionary<
		typename LOOKUP::Element::Element>
{
protected:
	using LookupElement = typename LOOKUP::Element;
	using T = typename LookupElement::Element;

private:
	using Super = MutableDictionary<T>;

public:
	using typename Super::CStrings;

	DictionaryImplBase();
	DictionaryImplBase(DictionaryImplBase &&);
	virtual ~DictionaryImplBase();

	virtual const CStrings &c_strings() const override {return c_stringcache_();}

	bool contains(const std::string &name) const override;
	bool contains(const char *name) const override;

	const T &at(const char *name) const override;
	const T &at(const std::string &name) const override;
	const T& at(size_t i) const {return lookup_.at(i);}

	T &at(const char *name);
	T &at(const std::string &name);
	T& at(size_t i) {return lookup_.at(i);}

	T& add(T &&element) override;
	T& replace(T &&element) override;
	T erase(const std::string &) override;
	T erase(const char *) override;

	size_t size() const override {return lookup_.size();}
	bool empty() const override {return lookup_.empty();}

	void clear() override;

	CStrings &c_strings() {return c_stringcache_();}

protected:
	using const_iterator = typename LOOKUP::const_iterator;

	const_iterator begin() const {return lookup_.begin();}
	const_iterator end() const {return lookup_.end();}

	void init(std::vector<T> &&);
	const LookupElement &add(LookupElement &&element);
	const LookupElement &replace(LookupElement &&element);
	const LookupElement &replace(const_iterator, LookupElement &&element);

	template <class STRING>
	const_iterator find(const STRING name) const {return lookup_.find(name);}

	LookupElement erase(const_iterator i);
	virtual void recalc() const;
	virtual void computeCStrings(CStrings &) const;

	CStrings c_strings_ = {nullptr};
	Cached<CStrings &, DictionaryImplBase> c_stringcache_;

private:
	LOOKUP lookup_;
};

template<class LOOKUP>
inline DictionaryImplBase<LOOKUP>::DictionaryImplBase(
		DictionaryImplBase &&moved) :
		c_stringcache_(std::move(moved.c_stringcache_), *this, c_strings_),
		lookup_(std::move(moved.lookup_)) {}

template<class LOOKUP>
typename DictionaryImplBase<LOOKUP>::T &
		DictionaryImplBase<LOOKUP>::at(const std::string &name)
{
	return lookup_.at(name).element();
}

template<class LOOKUP>
const typename DictionaryImplBase<LOOKUP>::T &
		DictionaryImplBase<LOOKUP>::at(const std::string &name) const
{
	return lookup_.at(name).element();
}

template<class LOOKUP>
typename DictionaryImplBase<LOOKUP>::T &
		DictionaryImplBase<LOOKUP>::at(const char *name)
{
	return lookup_.at(name).element();
}

template<class LOOKUP>
inline const typename DictionaryImplBase<LOOKUP>::T &
		DictionaryImplBase<LOOKUP>::at(const char *name) const
{
	return lookup_.at(name).element();
}

template<class LOOKUP>
inline typename DictionaryImplBase<LOOKUP>::T&
		DictionaryImplBase<LOOKUP>::add(T &&element) {
	c_stringcache_.invalidate();
	return add(LookupElement(std::move(element))).element();
}

template<class LOOKUP>
inline typename DictionaryImplBase<LOOKUP>::T&
		DictionaryImplBase<LOOKUP>::replace(T &&element)
{
	c_stringcache_.invalidate();
	return replace(LookupElement(std::move(element))).element();
}

template<class LOOKUP>
inline const typename DictionaryImplBase<LOOKUP>::LookupElement& DictionaryImplBase<
		LOOKUP>::add(LookupElement &&element)
{
	return lookup_.add(std::move(element));
}

template<class LOOKUP>
inline const typename DictionaryImplBase<LOOKUP>::LookupElement& DictionaryImplBase<
		LOOKUP>::replace(LookupElement &&element)
{
	return lookup_.replace(std::move(element));
}

template<class LOOKUP>
inline const typename DictionaryImplBase<LOOKUP>::LookupElement& DictionaryImplBase<
		LOOKUP>::replace(const_iterator const_iterator,
		LookupElement &&element)
{
	return lookup_.replace(const_iterator, std::move(element));
}


template<class LOOKUP>
inline bool DictionaryImplBase<LOOKUP>::contains(
		const std::string &name) const
{
	return lookup_.contains(name);
}

template<class LOOKUP>
inline bool DictionaryImplBase<LOOKUP>::contains(
		const char *name) const
{
	return lookup_.contains(name);
}

template<class LOOKUP>
inline DictionaryImplBase<LOOKUP>::DictionaryImplBase()
		: c_stringcache_(*this,
				&DictionaryImplBase::computeCStrings, c_strings_) {}

template<class LOOKUP>
inline DictionaryImplBase<LOOKUP>::~DictionaryImplBase() {}

template<class LOOKUP>
inline void DictionaryImplBase<LOOKUP>::recalc() const
{
	c_stringcache_.invalidate();
}

template<class LOOKUP>
inline void DictionaryImplBase<LOOKUP>::init(std::vector<T> &&v)
{
	for (
			auto i = std::make_move_iterator(v.begin());
			i != std::make_move_iterator(v.end());
			++i)
	{
		add(*i);
	}
}

template<class LOOKUP>
inline void DictionaryImplBase<LOOKUP>::clear()
{
	lookup_.clear();
	c_stringcache_.invalidate();
}

template<class LOOKUP>
inline typename DictionaryImplBase<LOOKUP>::T DictionaryImplBase<LOOKUP>::
		erase(const std::string &name)
{
	c_stringcache_.invalidate();
	return std::move(lookup_.erase(name)).move_element();
}

template<class LOOKUP>
inline typename DictionaryImplBase<LOOKUP>::T DictionaryImplBase<LOOKUP>::
		erase(const char *name)
{
	c_stringcache_.invalidate();
	return std::move(lookup_.erase(name)).move_element();
}

template<class LOOKUP>
inline typename DictionaryImplBase<LOOKUP>::LookupElement DictionaryImplBase<LOOKUP>::erase(
		const_iterator i)
{
	c_stringcache_.invalidate();
	return std::move(lookup_.erase(i));
}

template<class LOOKUP>
inline void DictionaryImplBase<LOOKUP>::computeCStrings(
		CStrings &c_strings) const
{
	c_strings.clear();

	for (auto i = lookup_.begin(); i != lookup_.end(); ++i) {
		c_strings.push_back(i -> name().c_str());
	}

	c_strings.push_back(nullptr);
}

}

#endif /* UTIL_LEXICALDICTIONARY_H_ */
