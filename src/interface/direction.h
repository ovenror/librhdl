/*
 * interfacedirection.h
 *
 *  Created on: Apr 4, 2021
 *      Author: ovenror
 */

#ifndef INTERFACE_DIRECTION_H_
#define INTERFACE_DIRECTION_H_

#include "rhdl/construction/c/types.h"

#include <cassert>
#include <ostream>

namespace rhdl {

enum class SingleDirection {OUT = RHDL_OUT, IN = RHDL_IN};

inline SingleDirection reversed(SingleDirection d)
{
	switch (d) {
	case SingleDirection::IN: return SingleDirection::OUT;
	case SingleDirection::OUT: return SingleDirection::IN;
	default:
		assert (0);
		return static_cast<SingleDirection>(-1);
	}
}


std::ostream &operator<<(std::ostream &os, SingleDirection dir);

}

#endif /* INTERFACE_DIRECTION_H_ */
