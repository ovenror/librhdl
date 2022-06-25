#ifndef TM_ELEMENT_H
#define TM_ELEMENT_H

#include "util/cached.h"
#include "representation/blocks/types.h"

#include <vector>
#include <memory>

namespace rhdl {
namespace spatial {

class Element
{
public:
	Element(unsigned int index);

	unsigned int index() const {return index_;}

	blocks::index_t position() const;
	void setPosition(blocks::index_t position);
	bool hasPosition() const {return has_position_;}

protected:
	void incrementIndex() {++index_;}

private:
	unsigned int index_;
	bool has_position_;
	blocks::index_t position_;
};

}
}
#endif // ELEMENT_H
