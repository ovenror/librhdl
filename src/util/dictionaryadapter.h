/*
 * liskovdictionary.h
 *
 *  Created on: Apr 19, 2025
 *      Author: js
 */

#ifndef SRC_UTIL_DICTIONARYADAPTER_H_
#define SRC_UTIL_DICTIONARYADAPTER_H_

#include "mutabledictionary.h"
#include <memory>

namespace rhdl {

namespace detail {

template <template <class> class DICT, class T, class CT>
class DictionaryAdapterBase : public Dictionary<CT> {
	using Super = Dictionary<CT>;

public:
	DictionaryAdapterBase(DICT<T> &dict) : dict_(dict) {}

	bool contains(const std::string &name) const override {return dict_.contains(name);}
	bool contains(const char *name) const override {return dict_.contains(name);}

	size_t size() const override {return dict_.size();}

	const typename Super::CStrings &c_strings() const {return dict_.c_strings();}

protected:
	DICT<T> &dict_;
};

}

template <class DICT, class CT>
class ConvertingDictionaryAdapter {
	static_assert(0, "DICT must be a Dictionary, and its template parameter "
			"must be convertible to CT");
};

template <template <class> class DICT, class T, class CT>
class ConvertingDictionaryAdapter<DICT<T>, CT>
		: public detail::DictionaryAdapterBase<DICT, T, CT>
{
	using Super = detail::DictionaryAdapterBase<DICT, T, CT>;

public:
	ConvertingDictionaryAdapter(DICT<T> &dict) : Super(dict) {}

	CT at(const char *name) const override {return Super::dict_.at(name);}
	CT at(const std::string &name) const override {return Super::dict_.at(name);}
};

template <class DICT>
class DereferencingDictionaryAdapter {
	static_assert(0, "DICT must be a Dictionary and its template parameter "
			"must implement operator*()");
};

template <template <class> class DICT, class T>
class DereferencingDictionaryAdapter<DICT<T*>>
		: public detail::DictionaryAdapterBase<DICT, T*, T&>
{
	using Super = detail::DictionaryAdapterBase<DICT, T*, T&>;

public:
	DereferencingDictionaryAdapter(DICT<T*> &dict) : Super(dict) {}

	T &at(const char *name) const override {return *Super::dict_.at(name);}
	T &at(const std::string &name) const override {return *Super::dict_.at(name);}
};

template <template <class> class DICT, class T>
class DereferencingDictionaryAdapter<DICT<std::unique_ptr<T>>>
		: public detail::DictionaryAdapterBase<DICT, std::unique_ptr<T>, T&>
{
	using Super = detail::DictionaryAdapterBase<DICT, std::unique_ptr<T>, T&>;

public:
	DereferencingDictionaryAdapter(DICT<std::unique_ptr<T>> &dict) : Super(dict) {}

	T &at(const char *name) const override {return *Super::dict_.at(name);}
	T &at(const std::string &name) const override {return *Super::dict_.at(name);}
};

}



#endif /* SRC_UTIL_DICTIONARYADAPTER_H_ */
