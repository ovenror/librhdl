/*
 * structuremode.h
 *
 *  Created on: May 13, 2025
 *      Author: js
 */

#ifndef INCLUDE_RHDL_CONSTRUCTION_STRUCTUREMODE_H_
#define INCLUDE_RHDL_CONSTRUCTION_STRUCTUREMODE_H_

#include <rhdl/construction/c/flags.h>

enum StructureMode {
	CREATE_STATELESS = static_cast<int>(F_CREATE_STATELESS),
	CREATE_STATEFUL = static_cast<int>(F_CREATE_STATEFUL),
	EXISTS = static_cast<int>(F_EXISTS)
};

#endif /* INCLUDE_RHDL_CONSTRUCTION_STRUCTUREMODE_H_ */
