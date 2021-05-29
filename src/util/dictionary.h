/*
 * dictionary.h
 *
 *  Created on: May 12, 2021
 *      Author: ovenror
 */

#ifndef UTIL_DICTIONARY_H_
#define UTIL_DICTIONARY_H_

#include "cached.h"

#include <set>
#include <tuple>
#include <vector>

namespace rhdl {

template <class T>
class Dictionary {
private:
	class Less;
	using CStrings = std::vector<const char *>;
public:
	using const_iterator = typename std::set<T>::const_iterator;
	using size_type = typename std::set<T>::size_type;

	Dictionary();

	std::pair<const_iterator, bool> insert(T element);

	T& at(const std::string &name);
	const T& at(const std::string &name) const;
	const_iterator find(const std::string &name) const;
	const_iterator begin() const;
	const_iterator end() const;
	const_iterator cbegin() const {return begin();}
	const_iterator cend() const {return end();}

	size_type size() const;

	const char *const *c_strings() const {return c_strings_().data();}

private:
	void compute_cstrings(CStrings &cs) const;

	Cached<CStrings, Dictionary> c_strings_;
	std::set<T, Less> set_;
};

template<class T>
Dictionary<T>::Dictionary() : c_strings_(*this, &Dictionary<T>::compute_cstrings)
{}

template<class T>
inline std::pair<typename std::set<T>::const_iterator, bool> Dictionary<T>::insert(T element)
{
	auto result = set_.insert(std::move(element));

	if (result.second)
		c_strings_.invalidate();

	return result;
}

template<class T>
const T& Dictionary<T>::at(const std::string& name) const
{
	const_iterator i;
	if ((i = find(name)) == end())
		throw std::out_of_range("Dictionary::at");

	return *i;
}

/*
 * ok, since names used for comparison are const
 */
template<class T>
T& Dictionary<T>::at(const std::string& name)
{
	return const_cast<T&>(static_cast<const Dictionary<T> *>(this) -> at(name));
}

template<class T>
inline typename std::set<T>::const_iterator Dictionary<T>::find(const std::string& name) const {
	return set_.find(name);
}

template<class T>
inline typename std::set<T>::const_iterator Dictionary<T>::begin() const
{
	return set_.begin();
}

template<class T>
inline typename std::set<T>::const_iterator Dictionary<T>::end() const
{
	return set_.end();
}

template<class T>
inline typename std::set<T>::size_type Dictionary<T>::size() const {
	return set_.size();
}

template<class T>
void Dictionary<T>::compute_cstrings(CStrings& cs) const {
	cs.clear();
	for (const T &element : set_) {
		cs.push_back(element -> name().c_str());
	}
	cs.push_back(nullptr);
}

template <class T>
class Dictionary<T>::Less {
public:
	using is_transparent = void;

	bool operator()(const T &lhs, const T &rhs) const {
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
};

}

#endif /* UTIL_DICTIONARY_H_ */
