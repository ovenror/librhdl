/*
 * dictionary.h
 *
 *  Created on: Sep 29, 2024
 *      Author: ovenror
 */

#ifndef SRC_UTIL_DICTIONARY_DICTIONARY_H_
#define SRC_UTIL_DICTIONARY_DICTIONARY_H_

#include "dictionarybase.h"
#include "util/util.h"

#include <memory>
#include <string>

namespace rhdl::dictionary {

namespace detail {
template <class T> class iterator;
template <class T> class iterator_impl;
}

template <class T>
class Dictionary : public Dictionary<const T> {
	using Super = Dictionary<const T>;

public:
	using iterator_impl = detail::iterator_impl<T>;
	using const_iterator_impl = typename Super::iterator_impl;
	using iterator = detail::iterator<T>;
	using const_iterator = typename Super::iterator;
	using ValueType = T;

	virtual ~Dictionary() {}

	virtual T &at(size_t) = 0;
	virtual T &at(const std::string &name) = 0;
	virtual T &at(const char *name) = 0;

	using Super::begin;
	using Super::end;
	iterator begin() {return iterator(begin_impl());}
	iterator end() {return iterator(end_impl());}

protected:
	using IterImplPtr = std::unique_ptr<iterator_impl>;

private:
	virtual IterImplPtr begin_impl() = 0;
	virtual IterImplPtr end_impl() = 0;
};

template <class T>
class Dictionary<const T> : public DictionaryBase
{
	static constexpr bool returnRef = !std::is_pointer_v<T>;

public:
	using iterator_impl = detail::iterator_impl<const T>;
	using const_iterator_impl = iterator_impl;
	using iterator = detail::iterator<const T>;
	using const_iterator = iterator;
	using ValueType = const T;

	virtual ~Dictionary() {};

	virtual const T &at(size_t) const = 0;
	virtual const T &at(const std::string &name) const = 0;
	virtual const T &at(const char *name) const = 0;

	iterator begin() const {return iterator(cbegin_impl());}
	iterator end() const {return iterator(cend_impl());}
	iterator cbegin() const {return iterator(cbegin_impl());}
	iterator cend() const {return iterator(cend_impl());}

protected:
	using IterImplPtr = std::unique_ptr<iterator_impl>;

private:
	virtual IterImplPtr cbegin_impl() const = 0;
	virtual IterImplPtr cend_impl() const = 0;
};

namespace detail {
template <class T>
class iterator {
public:
	using value_type = T;
	using reference = T &;
	using iterator_category = std::forward_iterator_tag;
	//using difference_type = typename ElementIterator::difference_type;

	iterator(iterator &&moved) : i_(std::move(moved.i_)) {}
	iterator(std::unique_ptr<iterator_impl<T>> i) : i_(std::move(i)) {}

	iterator &operator=(iterator &&moved) {
		i_(std::move(moved.i_));
		return *this;
	}

	iterator &operator++() {++*i_; return *this;}
	bool operator==(const iterator &other) const {return *i_ == *other.i_;}
	bool operator!=(const iterator &other) const {return !(*i_ == *other.i_);}

	T *operator->() const {return &**i_;}
	T &operator*() const {return **i_;}

private:
	std::unique_ptr<iterator_impl<T>> i_;
};

template <class T>
struct iterator_impl {
	virtual ~iterator_impl() {}
	virtual iterator_impl &operator++() = 0;

	template <class CONCRETE, std::enable_if_t<!std::is_same_v<CONCRETE, iterator_impl>, int> = 0>
	bool equalsConcrete(const CONCRETE &other) const {
		const CONCRETE *cthis = dynamic_cast<const CONCRETE *>(this);
		if (!cthis)
			return false;
		return cthis -> equals(other);
	}

	virtual bool operator==(const iterator_impl &other) const = 0;

	virtual T &operator*() const = 0;
	//virtual T *operator->() const = 0;
};

}

}

#endif /* SRC_UTIL_DICTIONARY_DICTIONARY_H_ */
