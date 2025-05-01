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

#include <functional>

namespace rhdl::dictionary::detail {
	template <class T>
	using OrderPointer = remove_any_pointer_t<T> *;

	template <class ArgT>
	OrderPointer<ArgT> orderPointer(const ArgT &element) {return &*element;}

	template <class T, class ORDER = void>
	class LookupElement : public LookupElement<T, void>{
		using Super = LookupElement<T, void>;
		using typename Super::CStrings;
		using order_iterator = typename ORDER::const_iterator;

	public:
		LookupElement(T &&element, size_t index = -1)
			: Super(std::move(element)), order_index_(-1) {}

		LookupElement(LookupElement &&moved)
			: Super(std::move(moved)),
  			  order_index_(moved.order_index_),
			  order_iterator_(std::move(moved.order_iterator_)) {}

#if 0
		LookupElement &operator=(LookupElement &&moved)
		{
			order_iterator_ = std::move(moved.order_iterator_);
			static_cast<Super &>(*this) = std::move(moved);
			return *this;
		}
#endif

		mutable typename CStrings::size_type order_index_;
		mutable order_iterator order_iterator_;
	};

	template <class T>
	class LookupElement<T, void> {
	protected:
		using CStrings = DictionaryBase::CStrings;

	public:
		using Element = T;

		LookupElement(T &&element)
			: element_(std::move(element)) {}

		LookupElement(LookupElement &&moved)
			: element_(std::move(moved.element_)) {}
#if 0
		LookupElement &operator=(LookupElement &&moved)
		{
			element_ = std::move(moved.element_);
			order_index_ = moved.order_index_;
			return *this;
		}
#endif
		T &element() const {return ref_;}
		//const T &element() const {return element_;}
		T move_element() {return std::move(element_);}
		const std::string &name() const {return element_ -> name();}
		const char *c_str() const {return name().c_str();}

		operator const std::string &() const {return name();}
		operator const char *() const {return c_str();}
		operator T&() const {return ref_;}
		//operator const T&() const {return element_;}

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

	private:
		T element_;
		T &ref_ = element_;
	};

	template <class C>
	constexpr bool containerReturnsConst = std::is_const_v<
			std::remove_reference_t<decltype(
					* std::declval<typename C::iterator>())>>;

	template <class C, class T>
	constexpr bool returnConst =
			containerReturnsConst<C> || std::is_const_v<T>;


#if 0
	template <class ORDER> constexpr bool hasLookupHelper(
			decltype(&ORDER::at)) {return true;}
	template <class ORDER> constexpr bool hasLookupHelper(...) {return false;}

	template <class ORDER>
	constexpr bool hasLookup = hasLookupHelper<ORDER>(nullptr);

	template <class T, class ORDER>
	using getLookupElement_t = std::conditional_t<
			hasLookup<ORDER>(), LookupElement<T>, LookupElement<T, ORDER>>;
#endif
}


#endif /* SRC_UTIL_DICTIONARY_DICTIONARYIMPLDEFS_H_ */
