/*
 * blocks2commands.h
 *
 *  Created on: May 23, 2021
 *      Author: ovenror
 */

#ifndef TRANSFORMATION_BLOCKS2COMMANDS_H_
#define TRANSFORMATION_BLOCKS2COMMANDS_H_

#include "typedtransformation.h"
#include "representation/txt/commands.h"

namespace rhdl {

class BlocksToCommands : public TypedTransformation<Blocks, Commands> {
public:
	//BlocksToCommands();
	virtual ~BlocksToCommands();
	Commands execute(const Blocks &source) const override;
};

} /* namespace rhdl */

#endif /* TRANSFORMATION_BLOCKS2COMMANDS_H_ */
