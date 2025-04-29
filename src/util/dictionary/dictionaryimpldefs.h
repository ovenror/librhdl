/*
 * dictionaryimpldefs.h
 *
 *  Created on: Apr 26, 2025
 *      Author: js
 */

#ifndef SRC_UTIL_DICTIONARY_DICTIONARYIMPLDEFS_H_
#define SRC_UTIL_DICTIONARY_DICTIONARYIMPLDEFS_H_

#include <util/dictionary/dictionary.h>
#include "util/any_pointer.h"

namespace rhdl::dictionary::detail {
	template <class T>
	using OrderPointer = remove_any_pointer_t<T> *;

	template <class ArgT>
	OrderPointer<ArgT> orderPointer(const ArgT &element) {return &*element;}

	template <class T, class ORDER = void>
	class LookupElement : public LookupElement<T, void>{
		using Super = LookupElement<T, void>;
		using Super::CStrings;
		using order_iterator = typename ORDER::const_iterator;

	public:
		LookupElement(T element, size_t index = -1)
			: Super(std::move(element), index) {}

		LookupElement(LookupElement &&moved)
			: Super(std::move(moved)),
			  order_iterator_(std::move(moved.order_iterator_)) {}

		LookupElement &operator=(LookupElement &&moved)
		{
			order_iterator_ = std::move(moved.order_iterator_);
			static_cast<Super &>(*this) = std::move(moved);
			return *this;
		}

		mutable order_iterator order_iterator_;
	};

	template <class T>
	class LookupElement<T, void> {
	protected:
		using CStrings = DictionaryBase::CStrings;
		using ReturnType = typename Dictionary<T>::ReturnType;

	public:
		using Element = T;

		LookupElement(T element, size_t index = -1)
			: element_(std::move(element)), order_index_(index) {}

		LookupElement(LookupElement &&moved)
			: element_(std::move(moved.element_)),
			  order_index_(moved.order_index_) {}

		LookupElement &operator=(LookupElement &&moved)
		{
			element_ = std::move(moved.element_);
			order_index_ = moved.order_index_;
			return *this;
		}

		//ReturnType element() const {return element_;}
		const T &element() const {return element_;}
		const std::string &name() const {return element_ -> name();}
		const char *c_str() const {return name().c_str();}

		operator const std::string &() const {return name();}
		operator const char *() const {return c_str();}
		operator T&() {return element_;}
		operator const T&() const {return element_;}

		bool operator<(const LookupElement &other) const
		{
			if (c_str() == other.c_str())
				return false;

			return (name() < other.name());
		}

		bool operator>(const LookupElement &other) const
		{
			if (c_str() == other.c_str())
				return false;

			return (name() > other.name());
		}

		bool operator==(const LookupElement &other) const
		{
			if (c_str() == other.c_str())
				return true;

			return (name() == other.name());
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

		bool operator==(const std::string &other) const
		{
			if (c_str() == other.c_str())
				return true;

			return name() == other;
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

		bool operator==(const char* other) const
		{
			if (c_str() == other)
				return true;

			return name() == other;
		}

		mutable typename CStrings::size_type order_index_;

	private:
		T element_;
	};

	template <class ORDER> constexpr bool hasLookupHelper(
			decltype(&ORDER::at)) {return true;}
	template <class ORDER> constexpr bool hasLookupHelper(...) {return false;}

	template <class ORDER>
	constexpr bool hasLookup() {return hasLookupHelper<ORDER>(nullptr);}

	template <class T, class ORDER>
	using getLookupElement_t = std::conditional_t<
			hasLookup<ORDER>(), LookupElement<T>, LookupElement<T, ORDER>>;
}


#endif /* SRC_UTIL_DICTIONARY_DICTIONARYIMPLDEFS_H_ */
