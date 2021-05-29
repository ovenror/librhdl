/*
 * interfacedirection.h
 *
 *  Created on: Apr 4, 2021
 *      Author: ovenror
 */

#ifndef INTERFACE_DIRECTION_H_
#define INTERFACE_DIRECTION_H_

#include <rhdl/construction/c/types.h>
#include <iostream>

namespace rhdl {

enum class InterfaceDirection {OUT = RHDL_OUT, IN = RHDL_IN};

std::ostream &operator<<(std::ostream &os, InterfaceDirection dir);

}

#endif /* INTERFACE_DIRECTION_H_ */
