/*
 * liskovdictionary.h
 *
 *  Created on: Apr 19, 2025
 *      Author: js
 */

#ifndef SRC_UTIL_DICTIONARYADAPTER_H_
#define SRC_UTIL_DICTIONARYADAPTER_H_

#include "dictionary.h"
#include "util/any_pointer.h"
#include <memory>

namespace rhdl {

namespace detail {

template <class T, class RT>
class DictionaryAdapterBase : public Dictionary<RT> {
	using Super = Dictionary<RT>;

public:
	DictionaryAdapterBase(const Dictionary<T> &dict) : dict_(dict) {}

	bool contains(const std::string &name) const override {return dict_.contains(name);}
	bool contains(const char *name) const override {return dict_.contains(name);}

	size_t size() const override {return dict_.size();}

	const typename Super::CStrings &c_strings() const {return dict_.c_strings();}

	const DictionaryBase *underlying() const override
	{
		return dict_.underlying();
	}

protected:
	const Dictionary<T> &dict_;
};

}

template <class T>
class DereferencingDictionaryAdapter
		: public detail::DictionaryAdapterBase<T, remove_any_pointer_t<T>>
{
	using Super = detail::DictionaryAdapterBase<T, remove_any_pointer_t<T>>;
	using typename Super::ReturnType;

public:
	DereferencingDictionaryAdapter(const Dictionary<T> &dict) : Super(dict) {}

	const ReturnType at(const char *name) const override {return *Super::dict_.at(name);}
	const ReturnType at(const std::string &name) const override {return *Super::dict_.at(name);}
};

template <class T, class CT>
class ConvertingDictionaryAdapter : public detail::DictionaryAdapterBase<T, CT>
{
	using Super = detail::DictionaryAdapterBase<T, CT>;
	using typename Super::ReturnType;

public:
	ConvertingDictionaryAdapter(const Dictionary<T> &dict) : Super(dict) {}

	ReturnType at(const char *name) const override {return Super::dict_.at(name);}
	ReturnType at(const std::string &name) const override {return Super::dict_.at(name);}
};

template <class T, class CT>
class DerefConvDictionaryAdapter : public detail::DictionaryAdapterBase<T, CT>
{
	using Super = detail::DictionaryAdapterBase<T, CT>;
	using typename Super::ReturnType;

public:
	DerefConvDictionaryAdapter(const Dictionary<T> &dict) : Super(dict) {}

	ReturnType at(const char *name) const override {return *Super::dict_.at(name);}
	ReturnType at(const std::string &name) const override {return *Super::dict_.at(name);}
};

#if 0
template <class DICT, bool DEREFERENCE_FIRST = false>
class PolymorphicDictionary : public DICT {
	template <class CT>
	operator ConvertingDictionaryAdapter<DICT, CT>() const {
		return ConvertingDictionaryAdapter<DICT, CT>(*this);
	}
};

template <template <class> class DICT, class T>
class PolymorphicDictionary<DICT<T>, true> : public DICT<T> {
	template <class CT>
	operator DerefConvDictionaryAdapter<DICT<T>, CT>() const {
		return DerefConvDictionaryAdapter<DICT<T>, CT>(*this);
	}
};
#endif

}



#endif /* SRC_UTIL_DICTIONARYADAPTER_H_ */
