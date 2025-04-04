/*
 * dictionary.h
 *
 *  Created on: May 12, 2021
 *      Author: ovenror
 */

#ifndef UTIL_LEXICALDICTIONARY_H_
#define UTIL_LEXICALDICTIONARY_H_

#include "cached.h"
#include "dictionary.h"

#include <rhdl/construction/constructionexception.h>

#include <set>
#include <tuple>
#include <vector>

namespace rhdl {

template <class T>
class LexicalDictionary : public Dictionary<T> {
private:
	class Less;
	using CStrings = std::vector<const char *>;

public:
	using iterator = typename std::set<T>::iterator;
	using const_iterator = typename std::set<T>::const_iterator;
	using size_type = typename std::set<T>::size_type;

	LexicalDictionary();
	LexicalDictionary(LexicalDictionary &&);
	virtual ~LexicalDictionary() {}

	std::pair<const_iterator, bool> insert(T element);

	const T& at(const char *name) const override;
	const T& at(const std::string &name) const override;

	const T &add(T element);
	const T &replace(T element);

	const_iterator find(const std::string& name) const;
	const_iterator begin() const;
	const_iterator end() const;
	const_iterator cbegin() const {return begin();}
	const_iterator cend() const {return end();}

	size_type size() const override;

	const std::vector<const char*> &c_strings() const override {return c_strings_();}

	iterator begin() {return set_.begin();}
	iterator end() {return set_.end();}
	void clear() {set_.clear(); c_strings_.invalidate();}

private:
	void compute_cstrings(CStrings &cs) const;

	Cached<CStrings, LexicalDictionary> c_strings_;
	std::set<T, Less> set_;
};

template<class T>
LexicalDictionary<T>::LexicalDictionary() :
		c_strings_(*this, &LexicalDictionary<T>::compute_cstrings) {}

template<class T>
inline std::pair<typename std::set<T>::const_iterator, bool> LexicalDictionary<T>::insert(T element)
{
	auto result = set_.insert(std::move(element));

	if (result.second)
		c_strings_.invalidate();

	return result;
}

template<class T>
const T& LexicalDictionary<T>::at(const std::string &name) const
{
	const_iterator i;
	if ((i = find(name)) == end())
		throw std::out_of_range("Dictionary::at(std::string &name)");

	return *i;
}

/*
 * ok, since names used for comparison are const
 */
/*
template<class T>
T& LexicalDictionary<T>::at(const std::string& name)
{
	return const_cast<T&>(static_cast<const LexicalDictionary<T> *>(this) -> at(name));
}
*/

/* speed boost for unique C strings */
template<class T>
const T& LexicalDictionary<T>::at(const char *name) const
{
	const_iterator i;
	if ((i = find(name)) == end())
		throw std::out_of_range("LexicalDictionary::at(const char *)");

	return *i;
}

template<class T>
inline typename std::set<T>::const_iterator LexicalDictionary<T>::find(const std::string& name) const {
	return set_.find(name);
}

template<class T>
inline typename std::set<T>::const_iterator LexicalDictionary<T>::begin() const
{
	return set_.begin();
}

template<class T>
inline typename std::set<T>::const_iterator LexicalDictionary<T>::end() const
{
	return set_.end();
}

template<class T>
inline typename std::set<T>::size_type LexicalDictionary<T>::size() const {
	return set_.size();
}

template<class T>
inline const T &LexicalDictionary<T>::add(T element) {
	auto [i, success] = insert(std::move(element));

	if (!success) {
		throw std::out_of_range("Element exists.");
	}

	return *i;
}

template<class T>
inline const T& LexicalDictionary<T>::replace(T element)
{
	auto i = find(element -> name());

	if (i != set_.end()) {
		throw std::out_of_range("No such element.");
	}

	set_.erase(i);
	return add(std::move(element));
}

template<class T>
inline LexicalDictionary<T>::LexicalDictionary(LexicalDictionary &&moved)
		: set_(std::move(moved.set_)),
		  c_strings_(*this, &LexicalDictionary<T>::compute_cstrings)
{}

template<class T>
void LexicalDictionary<T>::compute_cstrings(CStrings& cs) const {
	cs.clear();
	for (const T &element : set_) {
		cs.push_back(element -> name().c_str());
	}
	cs.push_back(nullptr);
}

template <class T>
class LexicalDictionary<T>::Less {
public:
	using is_transparent = void;

	bool operator()(const T &lhs, const T &rhs) const {
		if (lhs -> name().c_str() == rhs -> name().c_str())
			return false;

		return lhs -> name() < rhs -> name();
	}

	bool operator()(const T &lhs, const std::string &rhs) const {
		return lhs -> name() < rhs;
	}

	bool operator()(const std::string &lhs, const T &rhs) const {
		return lhs < rhs -> name();
	}

	bool operator()(const std::string &lhs, const std::string &rhs) const {
		return lhs < rhs;
	}

	bool operator()(const T &lhs, const char *rhs) const {
		if (lhs -> name().c_str() == rhs)
			return false;

		return lhs -> name() < rhs;
	}

	bool operator()(const char *lhs, const T &rhs) const {
		if (lhs == rhs -> name().c_str())
			return false;

		return lhs < rhs -> name();
	}
};

}

#endif /* UTIL_LEXICALDICTIONARY_H_ */
