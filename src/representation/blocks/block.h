#ifndef BLOCK_H
#define BLOCK_H

#include "representation/blocks/direction.h"

namespace rhdl {

class Block
{
public:
	enum Type : char {
		UNSET,
		TRANSPARENT,
		REDSTONE,
		TORCH,
		REPEATER,
		OPAQUE,
	};

	Block(Type type = UNSET, Direction orientation = RIGHT);

	Type type() const {return type_;}
	operator Type() const {return type();}

	Direction orientation() const {return orientation_;}

private:
	Type type_;
	Direction orientation_;
};

}

#endif // BLOCK_H
