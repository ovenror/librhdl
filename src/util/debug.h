/*
 * debug.h
 *
 *  Created on: Jun 25, 2021
 *      Author: ovenror
 */

#ifndef SRC_UTIL_DEBUG_H_
#define SRC_UTIL_DEBUG_H_

#ifdef __cplusplus
#include <iostream>

namespace rhdl {

enum LogLevel : unsigned int {LOGLVL_OFF, ERROR, LOG_REP, DEBUG};
constexpr LogLevel logLevel = LOG_REP;

#define LOG(lvl) if (lvl <= logLevel) std::cerr

}

#endif

#endif /* SRC_UTIL_DEBUG_H_ */
