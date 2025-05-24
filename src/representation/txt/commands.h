/*
 * commands.h
 *
 *  Created on: May 23, 2021
 *      Author: ovenror
 */

#ifndef REPRESENTATION_TXT_COMMANDS_H_
#define REPRESENTATION_TXT_COMMANDS_H_

#include "representation/representationbase.h"
#include "representation/blocks/types.h"

namespace rhdl::txt {

class Commands : public RepresentationBase<Commands> {
public:
	Commands(const blocks::Blocks &blocks, const std::string &name = "");

	virtual ~Commands();

	void moveTo(blocks::Vec pos);
	void out(std::ostream &os) const;

private:
	const blocks::Blocks &blocks_;
	blocks::Vec translation_;
};

std::ostream &operator<<(std::ostream &os, const Commands &commands);

} /* namespace rhdl::txt */

#endif /* REPRESENTATION_TXT_COMMANDS_H_ */
