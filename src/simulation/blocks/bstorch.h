#ifndef BSTORCH_H
#define BSTORCH_H

#include "bsdelay.h"

namespace rhdl::blocks {

class BSTorch : public BSDelay
{
public:
	BSTorch(Queue &q);

	void init_internal(const SimMap &smap, const Vec &pos, const Blocks& blocks) override;
	Block type() const override {return Block::TORCH;}

protected:
	void resetOnAfterSwitch() override;
	void updateOnAfterSwitch(char value) override;
};

}

#endif // BSTORCH_H
