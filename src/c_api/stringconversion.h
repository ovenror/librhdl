/*
 * stringconversion.h
 *
 *  Created on: Apr 11, 2025
 *      Author: js
 */

#ifndef SRC_C_API_STRINGCONVERSION_H_
#define SRC_C_API_STRINGCONVERSION_H_

#include <string>

namespace rhdl {

template <class VALUE_TYPE>
std::string to_string(VALUE_TYPE value);

} /* namespace rhdl */

#endif /* SRC_C_API_STRINGCONVERSION_H_ */
