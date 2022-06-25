#include "../tree/horizontal.h"

#include "../tree/layer.h"

namespace rhdl {

using blocks::Blocks;

namespace spatial {

Horizontal::Horizontal(unsigned int index)
	: Container(index)
{

}

blocks::index_t Horizontal::xpos() const
{
	return position();
}

blocks::index_t Horizontal::ypos() const
{
	assert (getLayer());
	return getLayer() -> asContainer().ypos();
}

}
}
