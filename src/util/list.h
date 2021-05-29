#ifndef LIST_H
#define LIST_H

#include <type_traits>

namespace rhdl {

template <class... Types>
struct List {};

template <class element, class list>
struct Concat;

template <class element, class... elements>
struct Concat<element, List<elements...>> {
	using list = List<element, elements...>;
};

template <class type, class ...rest>
struct Length : public std::integral_constant<unsigned int, Length<rest...>::value + 1> {};

template <class type>
struct Length<type> : public std::integral_constant<unsigned int, 1> {};

}

#endif // LIST_H
