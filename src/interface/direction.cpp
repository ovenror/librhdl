/*
 * direction.cpp
 *
 *  Created on: Apr 4, 2021
 *      Author: ovenror
 */

#include "direction.h"

#include <cassert>

namespace rhdl {

std::ostream& operator<<(std::ostream& os, SingleDirection dir) {
	switch (dir) {
	case SingleDirection::IN:
		os << "IN(" << (int) dir << ")";
		break;
	case SingleDirection::OUT:
		os << "OUT(" << (int) dir << ")";
		break;
	default:
		assert (0);
	}

	return os;
}

}
