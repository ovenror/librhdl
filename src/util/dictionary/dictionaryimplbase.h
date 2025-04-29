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

namespace rhdl::dictionary {

template <class LOOKUP>
class DictionaryImplBase : public LOOKUP, public MutableDictionary<
		typename LOOKUP::Element::Element>
{
protected:
	using LookupElement = typename LOOKUP::Element;

private:
	using T = typename LookupElement::Element;
	using Super = MutableDictionary<T>;

public:
	using typename Super::CStrings;
	using typename Super::ReturnType;

	DictionaryImplBase();
	DictionaryImplBase(DictionaryImplBase &&);
	virtual ~DictionaryImplBase();

	virtual const CStrings &c_strings() const override {return c_stringcache_();}

	bool contains(const std::string &name) const override;
	bool contains(const char *name) const override;

	ReturnType at(const char *name) const override;
	ReturnType at(const std::string &name) const override;

	const T &add(T element) override;
	const T &replace(T element) override;

	size_t size() const override {return LOOKUP::size();}

	void clear() override;

	CStrings &c_strings() {return c_stringcache_();}

protected:
	void init(std::vector<T> &&);
	using LOOKUP::add;
	using LOOKUP::replace;
	using LOOKUP::find;
	virtual void recalc() const;
	virtual void computeCStrings(CStrings &) const;

	CStrings c_strings_ = {nullptr};
	Cached<CStrings &, DictionaryImplBase> c_stringcache_;
};

template<class LOOKUP>
inline DictionaryImplBase<LOOKUP>::DictionaryImplBase(
		DictionaryImplBase &&moved) :
		c_stringcache_(std::move(moved.c_stringcache_), *this, c_strings_) {}

template<class LOOKUP>
typename DictionaryImplBase<LOOKUP>::ReturnType
		DictionaryImplBase<LOOKUP>::at(const std::string &name) const
{
	return LOOKUP::at(name).element();
}

/* speed boost for unique C strings */
template<class LOOKUP>
typename DictionaryImplBase<LOOKUP>::ReturnType
		DictionaryImplBase<LOOKUP>::at(const char *name) const
{
	auto &e = LOOKUP::at(name).element();
	return LOOKUP::at(name).element();
}

template<class LOOKUP>
inline const typename DictionaryImplBase<LOOKUP>::T &
		DictionaryImplBase<LOOKUP>::add(T element) {
	return add(LookupElement(std::move(element))).element();
}

template<class LOOKUP>
inline const typename DictionaryImplBase<LOOKUP>::T &
		DictionaryImplBase<LOOKUP>::replace(T element)
{
	return replace(LookupElement(std::move(element))).element();
}

template<class LOOKUP>
inline bool DictionaryImplBase<LOOKUP>::contains(
		const std::string &name) const
{
	return LOOKUP::contains(name);
}

template<class LOOKUP>
inline bool DictionaryImplBase<LOOKUP>::contains(
		const char *name) const
{
	return LOOKUP::contains(name);
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
	LOOKUP::clear();
	c_stringcache_.invalidate();
}

template<class LOOKUP>
inline void DictionaryImplBase<LOOKUP>::computeCStrings(
		CStrings &c_strings) const
{
	c_strings.clear();

	for (const auto &element : *this) {
		c_strings.push_back(element.name().c_str());
	}

	c_strings.push_back(nullptr);
}



}

#endif /* UTIL_LEXICALDICTIONARY_H_ */
