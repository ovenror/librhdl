/*
 * remove_any_pointer.h
 *
 *  Created on: Apr 19, 2025
 *      Author: js
 */

#ifndef SRC_UTIL_ANY_POINTER_H_
#define SRC_UTIL_ANY_POINTER_H_

#include <memory>

namespace rhdl {

template <class PT>
struct remove_any_pointer {
	using type = std::remove_pointer_t<PT>;
};

template <class Pointee>
struct remove_any_pointer<std::unique_ptr<Pointee>> {
	using type = Pointee;
};

template <class Pointee>
struct remove_any_pointer<const std::unique_ptr<Pointee>> {
	using type = Pointee;
};

template <class PT>
using remove_any_pointer_t = typename remove_any_pointer<PT>::type;

template <class PT>
struct is_any_pointer : public std::is_pointer<PT>{};

template <class Pointee>
struct is_any_pointer<std::unique_ptr<Pointee>> : std::true_type {};

template <class Pointee>
struct is_any_pointer<const std::unique_ptr<Pointee>> : std::true_type {};

template <class PT>
constexpr bool is_any_pointer_v = is_any_pointer<PT>();

}

#endif /* SRC_UTIL_ANY_POINTER_H_ */
