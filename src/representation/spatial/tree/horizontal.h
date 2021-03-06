#ifndef HORIZONTAL_H
#define HORIZONTAL_H

#include "../tree/container.h"

namespace rhdl {
namespace spatial {

class Horizontal : public Container
{
public:
	Horizontal(unsigned int index);

	blocks::index_t xpos() const override;
	blocks::index_t ypos() const override;
};

}
}

#endif // HORIZONTAL_H
