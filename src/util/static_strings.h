#ifndef STATIC_STRINGS_H
#define STATIC_STRINGS_H

#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>

#include <array>
#include <iostream>

namespace rhdl {

#define TOSTRING(strng) Truncate<StaticString<>, STRING_TO_CHARS_ANY(#strng)>::RESULT

template <unsigned int N>
constexpr char get_ch (char const (&s) [N], unsigned int i)
{
	return i >= N ? '\0' : s[i];
}

#define STRING_TO_CHARS_EXTRACT(z, n, data) \
		BOOST_PP_COMMA_IF(n) get_ch(data, n)

#define STRING_TO_CHARS(STRLEN, STR)  \
		BOOST_PP_REPEAT(STRLEN, STRING_TO_CHARS_EXTRACT, STR)

#define STRING_TO_CHARS_ANY(STR) \
		STRING_TO_CHARS(100, STR)


template <char... chars>
struct CLen;

template <char first, char... rest>
struct CLen<first, rest...> {
		constexpr static size_t result() {return CLen<rest...>::result()+1;}
};

template <>
struct CLen<> {
		constexpr static size_t result() {return 0;}
};

//only instantiated if the template parameter list is empty
template <class... T>
struct SStringList {
		constexpr static size_t len() {return 0;}
		static void test() {}
};

template<class T0, class... T>
struct SStringList<T0, T...> {
		using TAIL = SStringList<T...>;
		constexpr static size_t len() {return TAIL::len()+1;}

		static void test()
		{
				T0::test();
				TAIL::test();
		}
};

template <char... strng>
struct StaticString {
		constexpr static size_t len() {return CLen<strng...>::result();}
		constexpr static std::array<char, len()+1> chars() {return {strng..., 0};}

		static void test()
		{
				char msg[] = {strng...};
				std::cout << msg << std::endl;
		}
};

template <class StringInstance, char first, char... strng>
struct Truncate;

template <char... collected, char first, char... strng>
struct Truncate<StaticString<collected...>, first, strng...> {
		using RESULT = typename Truncate<StaticString<collected..., first>, strng...>::RESULT;
};

template <char... collected, char... strng>
struct Truncate<StaticString<collected...>, 0, strng...> {
		using RESULT = StaticString<collected...>;
};

template <class accu, class current, char... chars>
struct Split2;

template<class... strings, char... current_chars, char next_char, char... chars>
struct Split2<SStringList<strings...>, StaticString<current_chars...>, next_char, chars...> {
		using RESULT = typename Split2<SStringList<strings...>, StaticString<current_chars..., next_char>, chars...>::RESULT;
};

template<class... strings, char... current_chars, char... chars>
struct Split2<SStringList<strings...>, StaticString<current_chars...>, '.', chars...> {
		using RESULT = typename Split2<SStringList<strings..., StaticString<current_chars...> >, StaticString<>, chars...>::RESULT;
};

template<class... strings, char... current_chars>
struct Split2<SStringList<strings...>, StaticString<current_chars...> > {
		using RESULT = SStringList<strings..., StaticString<current_chars...> >;
};

template <class T>
struct Split;

template <char... chars>
struct Split<StaticString<chars...> > {
		using RESULT = typename Split2<SStringList<>, StaticString<>, chars...>::RESULT;
};

}

#endif // STATIC_STRINGS_H
