/*
 * blocks2commands.cpp
 *
 *  Created on: May 23, 2021
 *      Author: ovenror
 */

#include <transformation/blocks2commands.h>

namespace rhdl {

//BlocksToCommands::BlocksToCommands() {}
BlocksToCommands::~BlocksToCommands() {}

Commands BlocksToCommands::execute(const Blocks& source) const {
	return Commands(source.entity(), source, source.timing());
}

} /* namespace rhdl */
