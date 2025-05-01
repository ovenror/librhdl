/*
 * polymorphybase.h
 *
 *  Created on: May 2, 2025
 *      Author: js
 */

#ifndef SRC_UTIL_DICTIONARY_POLYMORPHYBASE_H_
#define SRC_UTIL_DICTIONARY_POLYMORPHYBASE_H_

#include "dictionary.h"
#include "dictionaryimpldefs.h"

namespace rhdl::dictionary::detail {

template <class DICT, class RT, bool CONST>
class IterImplBase : public Dictionary<RT>::iterator_impl
{
	using Dict2 = Dictionary<RT>;
	using Super = typename Dict2::iterator_impl;
	using This = IterImplBase<DICT, RT, CONST>;

protected:
	using iterator = std::conditional_t<CONST,
			typename DICT::const_iterator, typename DICT::iterator>;

public:
	IterImplBase(iterator i) : i_(std::move(i)) {}
	virtual ~IterImplBase() {}

	IterImplBase &operator++() override {++i_; return *this;}
	bool operator==(const Super &other) const override {return other.template equalsConcrete<This>(*this);}
	bool equals(const IterImplBase &other) const {return i_ == other.i_;}

protected:
	iterator i_;
};

template <class DICT, class RT, bool CONST>
class ConvIterImpl : public IterImplBase<DICT, RT, CONST>
{
	using Super = IterImplBase<DICT, RT, CONST>;
	using typename Super::iterator;
	using typename Super::ReturnType;

public:
	ConvIterImpl(iterator i) : Super(std::move(i)) {}
	virtual ~ConvIterImpl() {}

	RT &operator*() const override {return *Super::i_;}
	//RT *operator->() const override {return &*Super::i_;}
};

template <class DICT, class RT, bool CONST>
class DerefIterImpl : public IterImplBase<DICT, RT, CONST>
{
	using Super = IterImplBase<DICT, RT, CONST>;
	using typename Super::iterator;

public:
	DerefIterImpl(iterator i) : Super(std::move(i)) {}
	virtual ~DerefIterImpl() {}

	RT &operator*() const override {return **Super::i_;}
	//RT *operator->() const override {return &**Super::i_;}
};

template <class DICT, class RT, bool DEREF>
using IterImpl = std::conditional_t<DEREF,
		DerefIterImpl<DICT, RT, false>, ConvIterImpl<DICT, RT, false>>;

template <class DICT, class RT, bool DEREF>
using ConstIterImpl = std::conditional_t<DEREF,
		DerefIterImpl<DICT, const RT, true>, ConvIterImpl<DICT, const RT, true>>;

template <class DICT, class RT, bool DEREF>
class PolymorphyBase : public Dictionary<RT> {
	using Super = Dictionary<RT>;
	using IterImplPtr = typename Dictionary<RT>::IterImplPtr;
	using ConstIterImplPtr = typename Dictionary<const RT>::IterImplPtr;
	using typename Super::iterator_impl;

public:
	PolymorphyBase(DICT &dict) : dict_(dict) {}
	virtual ~PolymorphyBase() {}

	bool contains(const std::string &name) const override {return dict_.contains(name);}
	bool contains(const char *name) const override {return dict_.contains(name);}

	size_t size() const override {return dict_.size();}
	bool empty() const override {return dict_.empty();}

	const typename Super::CStrings &c_strings() const {return dict_.c_strings();}

	const DictionaryBase *underlying() const override
	{
		return dict_.underlying();
	}

private:
	using II = IterImpl<DICT, RT, DEREF>;
	using CII = ConstIterImpl<DICT, RT, DEREF>;

	IterImplPtr begin_impl() {return std::make_unique<II>(dict_.begin());}
	IterImplPtr end_impl() {return std::make_unique<II>(dict_.end());}
	ConstIterImplPtr cbegin_impl() const override {return std::make_unique<CII>(dict_.cbegin());}
	ConstIterImplPtr cend_impl() const override {return std::make_unique<CII>(dict_.cend());}

protected:
	DICT &dict_;
};

}




#endif /* SRC_UTIL_DICTIONARY_POLYMORPHYBASE_H_ */
