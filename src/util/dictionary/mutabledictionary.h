/*
 * mutabledictionary.h
 *
 *  Created on: Apr 18, 2025
 *      Author: js
 */

#ifndef SRC_UTIL_DICTIONARY_MUTABLEDICTIONARY_H_
#define SRC_UTIL_DICTIONARY_MUTABLEDICTIONARY_H_

#include "dictionaryadapter.h"
#include "util/util.h"

namespace rhdl::dictionary {
namespace detail {

template<typename C, typename R, typename... A>
R ret(R(C::*)(A...) const);

template <class T> static constexpr bool hasNameHelper(decltype(&remove_any_pointer_t<T>::name)) {return true;}
template <class T> static constexpr bool hasNameHelper(...) {return false;}
template <class T> constexpr bool hasName = hasNameHelper<T>(nullptr);

template <class T, bool HASNAME>
struct CheckName {
	static constexpr bool ok = false;
};

template <class T>
struct CheckName<T, true> {
	static constexpr bool ok = std::is_same_v<decltype(ret(&remove_any_pointer_t<T>::name)), const std::string &>;
};

template <class T> constexpr bool nameOK = CheckName<T, hasName<T>>::ok;
}

template <class T>
class MutableDictionary : public Dictionary<T> {
	using Super = Dictionary<T>;

	static_assert(detail::hasName<T>);
	static_assert(detail::nameOK<T>);

	template <class CT = remove_any_pointer_t<T>>
	using Deref = DereferencingDictionaryAdapter<Dictionary<T>, CT>;

	template <class CT>
	using Conv = ConvertingDictionaryAdapter<Dictionary<T>, CT>;

public:
	using typename Super::CStrings;

	MutableDictionary() {}
	virtual ~MutableDictionary() {}

	virtual T &add(T &&element) = 0;
	virtual T &replace(T &&element) = 0;
	virtual T erase(const std::string &) = 0;
	virtual T erase(const char *) = 0;

	virtual void clear() = 0;

#if 0
	template <class CT, std::enable_if_t<!std::is_same_v<T, CT>, bool> dummy = true>
	auto converter() const {return Conv<CT>(*this);}

	template <class CT, std::enable_if_t<!std::is_same_v<T, CT>, bool> dummy = true>
	auto converterPtr() const {return std::make_unique<Conv<CT>>(*this);}

	template <std::enable_if_t<is_any_pointer_v<T>, bool> dummy = true>
	auto dereferencer() const {return Deref<>(*this);}

	template <std::enable_if_t<is_any_pointer_v<T>, bool> dummy = true>
	auto dereferencerPtr() const {return std::make_unique<Deref<>>(*this);}

	template <class CT, std::enable_if_t<!std::is_same_v<T, CT>, bool> dummy = true>
	auto derefConv() const {return Deref<CT>(*this);}

	template <class CT, std::enable_if_t<!std::is_same_v<T, CT>, bool> dummy = true>
	auto derefConvPtr() const {return std::make_unique<Deref<CT>>(*this);}

	template <class CT, std::enable_if_t<!std::is_same_v<Dictionary<T>, CT>, bool> dummy = true>
	operator ConvertingDictionaryAdapter<T, CT>() const {
		return converter<CT>();
	}
#endif
};

} /* namespace rhdl::dictionary */

#endif /* SRC_UTIL_DICTIONARY_MUTABLEDICTIONARY_H_ */
