#ifndef BSREPEATER_H
#define BSREPEATER_H

#include "bsdelay.h"

namespace rhdl::blocks {

class BSRepeater : public BSDelay
{
public:
	BSRepeater(Queue &q);

	void init_internal(const SimMap &smap, const Vec &pos, const Blocks& blocks) override;
	Block type() const {return Block::REPEATER;}

protected:
	void resetOnAfterSwitch() override;
	void updateOnAfterSwitch(char value) override;
};

}

#endif // BSREPEATER_H
