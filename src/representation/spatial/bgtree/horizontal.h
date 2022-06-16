#ifndef HORIZONTAL_H
#define HORIZONTAL_H

#include "container.h"

namespace rhdl {
namespace TM {

class Horizontal : public Container
{
public:
	Horizontal(unsigned int index);

	blocks::Blocks::index_t xpos() const override;
	blocks::Blocks::index_t ypos() const override;
};

}
}

#endif // HORIZONTAL_H