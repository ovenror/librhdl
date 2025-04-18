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
	static_assert(!std::is_same_v<T, CObject>);

	using Super = DictionaryBase<T>;
	using typename Super::const_iterator;
	using typename Super::CStrings;

public:
	LexicalDictionary() = default;
	LexicalDictionary(LexicalDictionary &&) = default;

	virtual ~LexicalDictionary() {}

	std::pair<const_iterator, bool> insert(T element) override;

private:
	void compute_cstrings(CStrings &cs) const override;
};

template <class T>
std::pair<typename LexicalDictionary<T>::const_iterator, bool> LexicalDictionary<T>::insert(T element)
{
	auto result = Super::insert(std::move(element));

	if (result.second)
		Super::invalidate_c_strings();

	return result;
}

template<class T>
void LexicalDictionary<T>::compute_cstrings(LexicalDictionary<T>::CStrings& cs) const {
	cs.clear();
	for (const T &element : *this) {
		cs.push_back(element -> name().c_str());
	}
	cs.push_back(nullptr);
}

}

#endif /* SRC_UTIL_LEXICALDICTIONARY_H_ */
