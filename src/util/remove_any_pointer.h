/*
 * remove_any_pointer.h
 *
 *  Created on: Apr 19, 2025
 *      Author: js
 */

#ifndef SRC_UTIL_REMOVE_ANY_POINTER_H_
#define SRC_UTIL_REMOVE_ANY_POINTER_H_

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

template <class PT>
using remove_any_pointer_T = typename remove_any_pointer<PT>::type;

}

#endif /* SRC_UTIL_REMOVE_ANY_POINTER_H_ */
