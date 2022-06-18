#include "../tree/element.h"

#include "../tree/node.h"
#include "../tree/nodegroup.h"

namespace rhdl {

using blocks::Blocks;

namespace spatial {

Blocks::index_t Element::position() const
{
	assert (has_position_);
	return position_;
}

void Element::setPosition(Blocks::index_t position)
{
	assert (!hasPosition());
	assert (position >= 0);

	position_ = position;
	has_position_ = true;
}

Element::Element(unsigned int index) :
	 index_(index), has_position_(false), position_(0)
{}

}
}