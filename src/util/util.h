#ifndef UTIL_H
#define UTIL_H

#include <utility>

class None;

namespace rhdl {
class Empty{};

template <bool CONST, class T>
using cond_const = std::conditional <CONST, const T, T>;
}



#endif // UTIL_H
