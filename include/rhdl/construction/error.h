/*
 * error.h
 *
 *  Created on: May 10, 2021
 *      Author: ovenror
 */

#ifndef INCLUDE_RHDL_CONSTRUCTION_ERROR_H_
#define INCLUDE_RHDL_CONSTRUCTION_ERROR_H_

namespace rhdl {

enum class Errorcode {
	#include <rhdl/construction/error.inc>
};

}


#endif /* INCLUDE_RHDL_CONSTRUCTION_ERROR_H_ */
