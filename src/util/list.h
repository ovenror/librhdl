#ifndef LIST_H
#define LIST_H

#include <type_traits>

namespace rhdl {

template <class... Types>
struct List {};

template <class pelement, class... elements>
struct Car {
	using element = pelement;
};

template <class pelement, class... elements>
struct Car<List<pelement, elements...>> {
	using element = pelement;
};

template <>
struct Car<List<>> {
	static_assert("Cannot take first element from List<>.");
};

template <class element, class... elements>
struct Cdr {
	using list = List<elements...>;
};

template <class element, class... elements>
struct Cdr<List<element, elements...>> {
	using list = List<elements...>;
};

template <>
struct Cdr<List<>> {
	static_assert("Cannot take CDR from List<>.");
};

template <class... elements_or_lists>
struct Concat;

template <class element, class... elements, class... elements_or_lists>
struct Concat<element, List<elements...>, elements_or_lists...> {
	using list = typename Concat<List<element, elements...>, elements_or_lists...>::list;
};

template <class... elements, class element, class... elements_or_lists>
struct Concat<List<elements...>, element, elements_or_lists...> {
	using list = typename Concat<List<elements..., element>, elements_or_lists...>::list;
};

template <class... elements1, class... elements2, class... elements_or_lists>
struct Concat<List<elements1...>, List<elements2...>, elements_or_lists...> {
	using list = typename Concat<List<elements1..., elements2...>, elements_or_lists...>::list;
};

template <class... elements>
struct Concat<List<elements...>> {
	using list = List<elements...>;
};

template <class element>
struct Concat<element> {
	using list = List<element>;
};

template <class pelement, class... elements>
struct Last {
	using element = typename Last<elements...>::element;
};

template <class pelement>
struct Last<pelement> {
	using element = pelement;
};

template <class pelement, class... elements>
struct Last<List<pelement, elements...>> {
	using element = typename Last<elements...>::element;
};

template <class pelement>
struct Last<List<pelement>> {
	using element = pelement;
};

template <>
struct Last<List<>> {
	static_assert("Cannot take last element from List<>.");
};


template <class type, class... rest>
struct Length : public std::integral_constant<unsigned int, Length<rest...>::value + 1> {};

template <class type>
struct Length<type> : public std::integral_constant<unsigned int, 1> {};

}

#endif // LIST_H
