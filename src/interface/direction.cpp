/*
 * direction.cpp
 *
 *  Created on: Apr 4, 2021
 *      Author: ovenror
 */

#include "direction.h"

#include <cassert>

namespace rhdl {

std::ostream& operator<<(std::ostream& os, InterfaceDirection dir) {
	switch (dir) {
	case InterfaceDirection::IN:
		os << "IN(" << (int) dir << ")";
		break;
	case InterfaceDirection::OUT:
		os << "OUT(" << (int) dir << ")";
		break;
	default:
		assert (0);
	}

	return os;
}

}
