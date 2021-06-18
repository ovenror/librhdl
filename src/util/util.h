#ifndef UTIL_H
#define UTIL_H

#include <utility>
#include <memory>

class None;

namespace rhdl {
class Empty{};

template <bool CONST, class T>
using cond_const = std::conditional <CONST, const T, T>;
}

template <class T>
struct remove_unique_ptr {
	using type = T;
};

template <class T>
struct remove_unique_ptr<std::unique_ptr<T>> {
	using type = T;
};

#endif // UTIL_H
