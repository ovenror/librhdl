/*
 * lexicaldictionary.h
 *
 *  Created on: Apr 18, 2025
 *      Author: js
 */

#ifndef SRC_UTIL_LEXICALDICTIONARY_H_
#define SRC_UTIL_LEXICALDICTIONARY_H_

#include "dictionarybase.h"

namespace rhdl {

template <class T>
class LexicalDictionary : public DictionaryBase<T> {
	using Super = DictionaryBase<T>;
	using typename Super::const_iterator;
	using typename Super::CStrings;

public:
	LexicalDictionary();
	LexicalDictionary(LexicalDictionary &&);

	virtual ~LexicalDictionary() {}

	virtual const CStrings &c_strings() const override {return c_stringcache_();}

	const T &add(T element) override;
	const T &replace(T element) override;

	void clear() override;

private:
	void compute_cstrings(CStrings &cs) const;

	Cached<CStrings &, LexicalDictionary> c_stringcache_;
};

template<class T>
inline LexicalDictionary<T>::LexicalDictionary()
		: c_stringcache_(*this, &LexicalDictionary::compute_cstrings,
				Super::c_strings())
{
}

template<class T>
inline LexicalDictionary<T>::LexicalDictionary(LexicalDictionary &&moved)
		: c_stringcache_(
				std::move(moved.c_stringcache_), *this, Super::c_strings()) {}

template<class T>
void LexicalDictionary<T>::compute_cstrings(LexicalDictionary<T>::CStrings& cs) const {
	cs.clear();
	for (const T &element : *this) {
		cs.push_back(element -> name().c_str());
	}
	cs.push_back(nullptr);
}

template<class T>
inline void LexicalDictionary<T>::clear()
{
	Super::clear();
	c_stringcache_.invalidate();
}

template<class T>
inline const T& LexicalDictionary<T>::add(T element)
{
	c_stringcache_.invalidate();
	return Super::add(std::move(element));
}

template<class T>
inline const T& LexicalDictionary<T>::replace(T element)
{
	c_stringcache_.invalidate();
	return Super::replace(std::move(element));
}

}

#endif /* SRC_UTIL_LEXICALDICTIONARY_H_ */
