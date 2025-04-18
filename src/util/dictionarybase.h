/*
 * dictionary.h
 *
 *  Created on: May 12, 2021
 *      Author: ovenror
 */

#ifndef UTIL_LEXICALDICTIONARY_H_
#define UTIL_LEXICALDICTIONARY_H_

#include "cached.h"
#include "mutabledictionary.h"

#include <rhdl/construction/constructionexception.h>

#include <set>
#include <tuple>
#include <vector>

namespace rhdl {

template <class T>
class DictionaryBase : public MutableDictionary<T> {
private:
	using Super = MutableDictionary<T>;

public:
	class Less;
	using typename Super::CStrings;

	using size_type = typename std::set<T>::size_type;
	using iterator = typename std::set<T>::iterator;
	using const_iterator = typename std::set<T>::const_iterator;

	DictionaryBase();
	DictionaryBase(DictionaryBase &&);
	virtual ~DictionaryBase() {}

	bool contains(const std::string &name) const override;
	bool contains(const char *name) const override;

	const T& at(const char *name) const override;
	const T& at(const std::string &name) const override;

	const T &add(T element) override;
	const T &replace(T element) override;

	template <class ArgT>
	iterator find(ArgT nameOrT);

	template <class ArgT>
	const_iterator find(ArgT nameOrT) const;

	iterator begin() {return set_.begin();}
	iterator end() {return set_.end();}
	iterator begin() const {return set_.begin();}
	iterator end() const {return set_.end();}
	const_iterator cbegin() const {return begin();}
	const_iterator cend() const {return end();}

	virtual std::pair<const_iterator, bool> insert(T element);

	size_type size() const override;

	const CStrings &c_strings() const override {return c_strings_();}

	void clear() {set_.clear(); invalidate_c_strings();}

protected:
	void invalidate_c_strings() {c_strings_.invalidate();}

private:
	virtual void compute_cstrings(CStrings &cs) const = 0;

	Cached<CStrings, DictionaryBase> c_strings_;
	std::set<T, Less> set_;
};

template<class T>
DictionaryBase<T>::DictionaryBase() :
		c_strings_(*this, &DictionaryBase<T>::compute_cstrings) {}

template<class T>
inline std::pair<typename std::set<T>::const_iterator, bool> DictionaryBase<T>::insert(T element)
{
	return set_.insert(std::move(element));
}

template<class T>
const T& DictionaryBase<T>::at(const std::string &name) const
{
	const_iterator i;
	if ((i = find(name)) == cend())
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
const T& DictionaryBase<T>::at(const char *name) const
{
	const_iterator i;
	if ((i = find(name)) == cend())
		throw std::out_of_range("LexicalDictionary::at(const char *)");

	return *i;
}

template<class T>
inline typename std::set<T>::size_type DictionaryBase<T>::size() const {
	return set_.size();
}

template<class T>
inline const T &DictionaryBase<T>::add(T element) {
	auto [i, success] = insert(std::move(element));

	if (!success) {
		throw std::out_of_range("Element exists.");
	}

	return *i;
}

template<class T>
inline const T& DictionaryBase<T>::replace(T element)
{
	auto i = find(element -> name());

	if (i != set_.end()) {
		throw std::out_of_range("No such element.");
	}

	set_.erase(i);
	return add(std::move(element));
}

template<class T>
inline DictionaryBase<T>::DictionaryBase(DictionaryBase &&moved)
		: set_(std::move(moved.set_)),
		  c_strings_(std::move(moved.c_strings_), *this)
{}

template<class T>
inline bool DictionaryBase<T>::contains(const std::string &name) const
{
	return find(name) != cend();
}

template<class T>
inline bool DictionaryBase<T>::contains(const char *name) const
{
	return find(name) != cend();
}

template <class T>
template <class ArgT>
inline typename DictionaryBase<T>::iterator rhdl::DictionaryBase<T>::find(
		ArgT arg)
{
	return set_.find(arg);
}

template <class T>
template<class ArgT>
inline typename DictionaryBase<T>::const_iterator rhdl::DictionaryBase<T>::find(
		ArgT arg) const
{
	return set_.find(arg);
}

template <class T>
class DictionaryBase<T>::Less {
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
