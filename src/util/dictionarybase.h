/*
 * dictionary.h
 *
 *  Created on: May 12, 2021
 *      Author: ovenror
 */

#ifndef UTIL_LEXICALDICTIONARY_H_
#define UTIL_LEXICALDICTIONARY_H_

#include "cached.h"
#include <rhdl/construction/constructionexception.h>
#include <util/extensibledictionary.h>

#include <set>
#include <tuple>
#include <vector>
#include <cassert>

namespace rhdl {

template <class T>
class DictionaryBase : public ExtensibleDictionary<T> {
private:
	using Super = ExtensibleDictionary<T>;

public:
	class Less;
	friend class Less;
	using typename Super::CStrings;

	using size_type = typename std::set<T>::size_type;

#if 0
	using iterator = typename std::set<T>::iterator;
	using const_iterator = typename std::set<T>::const_iterator;
#endif

	DictionaryBase() = default;
	DictionaryBase(DictionaryBase &&) = default;
	virtual ~DictionaryBase() {}

	virtual const CStrings &c_strings() const override {return c_strings_;}

	bool contains(const std::string &name) const override;
	bool contains(const char *name) const override;

	const T& at(const char *name) const override;
	const T& at(const std::string &name) const override;

	const T &add(T element) override;
	virtual const T &replace(T element);

	size_type size() const override;

	virtual void clear() {set_.clear();}

protected:
	class SetElement {
	public:
		SetElement(T element, typename CStrings::size_type index = -1)
			: element_(std::move(element)), index_(index) {}
		SetElement(SetElement &&moved)
			: element_(std::move(moved.element_)), index_(moved.index_) {}

		SetElement &operator=(SetElement &&moved)
		{
			element_ = std::move(moved.element_);
			index_ = moved.index_;
			return *this;
		}

		const T &element() const {return element_;}
		const std::string &name() const {return element_ -> name();}
		const char *c_str() const {return name().c_str();}

		operator const std::string &() const {return name();}
		operator const char *() const {return c_str();}
		operator T&() {return element_;}
		operator const T&() const {return element_;}
		typename CStrings::size_type &index() const {return index_;}

		bool operator<(const SetElement &other) const
		{
			if (c_str() == other.c_str())
				return false;

			return (name() < other.name());
		}

		bool operator>(const SetElement &other) const
		{
			if (c_str() == other.c_str())
				return false;

			return (name() > other.name());
		}

		bool operator<(const std::string &other) const
		{
			if (c_str() == other.c_str())
				return false;

			return name() < other;
		}

		bool operator>(const std::string &other) const
		{
			if (c_str() == other.c_str())
				return false;

			return name() > other;
		}

		bool operator<(const char* other) const
		{
			if (c_str() == other)
				return false;

			return name() < other;
		}

		bool operator>(const char* other) const
		{
			if (c_str() == other)
				return false;

			return name() > other;
		}

	private:
		T element_;
		mutable typename CStrings::size_type index_;
	};

	virtual const SetElement &add(SetElement element);

	using iterator = typename std::set<SetElement>::iterator;
	using const_iterator = typename std::set<SetElement>::const_iterator;

	std::pair<iterator, bool> insert(T element);
	CStrings &c_strings() {return c_strings_;}

	iterator begin() {return set_.begin();}
	iterator end() {return set_.end();}
	const_iterator begin() const {return set_.begin();}
	const_iterator end() const {return set_.end();}

	template <class ArgT>
	iterator find(ArgT nameOrT);

	template <class ArgT>
	const_iterator find(ArgT nameOrT) const;

	CStrings c_strings_;
	std::set<SetElement, Less> set_;
};

#if 0
template <class T>
bool operator<(const std::string &lhs, const typename DictionaryBase<T>::SetElement &rhs)
{
	if (lhs.c_str() == rhs.c_str())
		return false;

	return lhs < rhs.name();
}

template <class T>
bool operator<(const char *lhs, const typename DictionaryBase<T>::SetElement &rhs)
{
	if (lhs == rhs.c_str())
		return false;

	return lhs < rhs.name();
}
#endif

template <class T>
inline std::pair<typename DictionaryBase<T>::iterator, bool> DictionaryBase<T>::insert(T element)
{
	auto iter = set_.emplace(std::move(element));
	iter -> index() = std::distance(set_.begin(), iter);
	return *iter;
}

template<class T>
const T& DictionaryBase<T>::at(const std::string &name) const
{
	const_iterator i;
	if ((i = set_.find(name)) == set_.end())
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
	if ((i = set_.find(name)) == set_.end())
		throw std::out_of_range("LexicalDictionary::at(const char *)");

	return *i;
}

template<class T>
inline typename std::set<T>::size_type DictionaryBase<T>::size() const {
	return set_.size();
}


template<class T>
inline const typename DictionaryBase<T>::SetElement& DictionaryBase<T>::add(
		SetElement element)
{
	auto [i, success] = set_.insert(std::move(element));

	if (!success) {
		throw std::out_of_range("Element exists.");
	}

	return *i;
}

template<class T>
inline const T &DictionaryBase<T>::add(T element) {
	return add(SetElement(std::move(element)));
}

template<class T>
inline const T &DictionaryBase<T>::replace(T element)
{
	SetElement se(std::move(element));
	auto i = set_.find(se);

	if (i == set_.end()) {
		throw std::out_of_range("No such element.");
	}

	auto nh = set_.extract(i);
	auto &ref = nh.value();
	auto index = ref.index();
	se.index() = index;
	ref = std::move(se);
	c_strings_[index] = ref;
	auto [iter, inserted, node] = set_.insert(std::move(nh));
	assert (inserted);
	return *iter;
}

template<class T>
inline bool DictionaryBase<T>::contains(const std::string &name) const
{
	return set_.find(name) != set_.end();
}

template<class T>
inline bool DictionaryBase<T>::contains(const char *name) const
{
	return set_.find(name) != set_.end();
}

template <class T>
class DictionaryBase<T>::Less {
	using ValueRef = const typename DictionaryBase<T>::SetElement &;

	template <class ArgT>
	using enable = std::enable_if_t<
			!std::is_same_v<const ArgT &, ValueRef>, bool>;

public:
	using is_transparent = void;

	bool operator()(ValueRef lhs, ValueRef rhs) const {
		return lhs < rhs;
	}

	template <class RHS, enable<RHS> dummy = true>
	bool operator()(ValueRef lhs, RHS rhs) const {
		return lhs < rhs;
	}

	template <class LHS, enable<LHS> dummy = true>
	bool operator()(LHS lhs, ValueRef rhs) const {
		return rhs > lhs;
	}

#if 0
	bool operator()(const T &lhs, const T &rhs) const {
		if (lhs -> name().c_str() == rhs -> name().c_str())
			return false;

		return lhs -> name() < rhs -> name();
		return lhs < rhs;
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
#endif
};

}

#endif /* UTIL_LEXICALDICTIONARY_H_ */
