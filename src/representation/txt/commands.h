/*
 * commands.h
 *
 *  Created on: May 23, 2021
 *      Author: ovenror
 */

#ifndef REPRESENTATION_TXT_COMMANDS_H_
#define REPRESENTATION_TXT_COMMANDS_H_

#include "representation/representationbase.h"
#include "representation/blocks/blocks.h"

namespace rhdl {

class Commands : public RepresentationBase<Commands> {
public:
	Commands(const Blocks &blocks);
	Commands(const Entity &entity, const Blocks &blocks,
	         const Timing *timing);

	virtual ~Commands();

	std::unique_ptr<Simulator> makeSimulator(bool use_behavior) const override;

	void moveTo(Blocks::Vec pos);
	void out(std::ostream &os) const;

private:
	const Blocks &blocks_;
	Blocks::Vec translation_;
};

std::ostream &operator<<(std::ostream &os, const Commands &commands);

} /* namespace rhdl */

#endif /* REPRESENTATION_TXT_COMMANDS_H_ */
