#include "horizontal.h"
#include "layer.h"

namespace rhdl {

using blocks::Blocks;

namespace TM {

Horizontal::Horizontal(unsigned int index)
	: Container(index)
{

}

Blocks::index_t Horizontal::xpos() const
{
	return position();
}

Blocks::index_t Horizontal::ypos() const
{
	assert (getLayer());
	return getLayer() -> asContainer().ypos();
}

}
}
