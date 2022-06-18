#ifndef TM_ELEMENT_H
#define TM_ELEMENT_H

#include "util/cached.h"
#include "representation/blocks/blocks.h"

#include <vector>
#include <memory>

namespace rhdl {
namespace TM {

class Element
{
public:
	Element(unsigned int index);

	unsigned int index() const {return index_;}

	blocks::Blocks::index_t position() const;
	void setPosition(blocks::Blocks::index_t position);
	bool hasPosition() const {return has_position_;}

protected:
	void incrementIndex() {++index_;}

private:
	unsigned int index_;
	bool has_position_;
	blocks::Blocks::index_t position_;
};

}
}
#endif // ELEMENT_H
