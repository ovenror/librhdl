/*
 * liskovdictionary.h
 *
 *  Created on: Apr 19, 2025
 *      Author: js
 */

#ifndef SRC_UTIL_DICTIONARY_DICTIONARYADAPTER_H_
#define SRC_UTIL_DICTIONARY_DICTIONARYADAPTER_H_

#include <util/dictionary/dictionary.h>
#include "polymorphybase.h"
#include "util/any_pointer.h"
#include <memory>

namespace rhdl::dictionary {

namespace detail {
template <class DICT, class RT, bool DEREF>
using DictionaryAdapterBase = PolymorphyBase<DICT, RT, DEREF>;
}

template <class DICT, class CT = remove_any_pointer_t<typename DICT::ValueType>>
class DereferencingDictionaryAdapter
		: public detail::DictionaryAdapterBase<DICT, CT, true>
{
	using Super = detail::DictionaryAdapterBase<DICT, CT, true>;

public:
	DereferencingDictionaryAdapter(DICT &dict) : Super(dict) {}
	virtual ~DereferencingDictionaryAdapter() {}

	const CT &at(size_t i) const override {return *Super::dict_.at(i);}
	const CT &at(const char *name) const override {return *Super::dict_.at(name);}
	const CT &at(const std::string &name) const override {return *Super::dict_.at(name);}

	CT &at(size_t i) {return *Super::dict_.at(i);}
	CT &at(const char *name) {return *Super::dict_.at(name);}
	CT &at(const std::string &name) {return *Super::dict_.at(name);}
};

template <class DICT, class CT>
class ConvertingDictionaryAdapter
		: public detail::DictionaryAdapterBase<DICT, CT, false>
{
	using Super = detail::DictionaryAdapterBase<DICT, CT, false>;

public:
	ConvertingDictionaryAdapter(const DICT &dict) : Super(dict) {}
	virtual ~ConvertingDictionaryAdapter() {}

	const CT &at(size_t i) const override {return Super::dict_.at(i);}
	const CT &at(const char *name) const override {return Super::dict_.at(name);}
	const CT &at(const std::string &name) const override {return Super::dict_.at(name);}

	CT &at(size_t i) {return Super::dict_.at(i);}
	CT &at(const char *name) {return Super::dict_.at(name);}
	CT &at(const std::string &name) {return Super::dict_.at(name);}
};

}



#endif /* SRC_UTIL_DICTIONARY_DICTIONARYADAPTER_H_ */
