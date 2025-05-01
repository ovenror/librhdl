/*
 * emptydictionary.h
 *
 *  Created on: Apr 19, 2025
 *      Author: js
 */

#ifndef SRC_UTIL_DICTIONARY_EMPTYDICTIONARY_H_
#define SRC_UTIL_DICTIONARY_EMPTYDICTIONARY_H_

#include <util/dictionary/dictionary.h>
#include <cassert>

namespace rhdl::dictionary {

namespace detail {
template <class T> class LolIterator;
}

template <class T>
class EmptyDictionary : public Dictionary<T> {
	using Super = Dictionary<T>;
	using typename Super::CStrings;

public:
	virtual ~EmptyDictionary() {}

	const T& at(size_t) const override
	{
		throw std::out_of_range(at_error);
	}

	const T& at(const std::string &name) const override
	{
		throw std::out_of_range(at_error);
	}

	const T& at(const char *name) const override {
		throw std::out_of_range(at_error);
	}

	T& at(size_t)
	{
		throw std::out_of_range(at_error);
	}

	T& at(const std::string &name)
	{
		throw std::out_of_range(at_error);
	}

	T& at(const char *name) {
		throw std::out_of_range(at_error);
	}

	bool contains(const std::string &name) const override {return false;}
	bool contains(const char *name) const override {return false;}

	size_t size() const override {return 0;}

	virtual const CStrings &c_strings() const {return c_strings_;}

protected:
	static constexpr auto at_error = "Empty dictionary is empty.";

private:
	using IterImplPtr = typename Dictionary<T>::IterImplPtr;
	using ConstIterImplPtr = typename Dictionary<const T>::IterImplPtr;
	IterImplPtr begin_impl() {return lolIterator();}
	IterImplPtr end_impl()  {return lolIterator();}
	ConstIterImplPtr cbegin_impl() const override {return constLolIterator();}
	ConstIterImplPtr cend_impl() const override {return constLolIterator();}

	IterImplPtr lolIterator() const {return std::make_unique<detail::LolIterator<T>>();}
	ConstIterImplPtr constLolIterator() const {return std::make_unique<detail::LolIterator<const T>>();}

	const std::vector<const char *> c_strings_ = {nullptr};
};

namespace detail {

template <class T>
class LolIterator : public Dictionary<T>::iterator_impl {
	using Super = typename Dictionary<T>::iterator_impl;

public:
	virtual ~LolIterator() {}

	LolIterator &operator++() override {return *this;}
	const T &operator*() const override {T *p = nullptr; return *p;}
	bool operator==(const Super &) const override {return true;}
	//const T *operator->() const override {return nullptr;}
};

}}



#endif /* SRC_UTIL_DICTIONARY_EMPTYDICTIONARY_H_ */
