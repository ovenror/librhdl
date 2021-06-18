#ifndef BSSWITCH_H
#define BSSWITCH_H

#include "blocksim.h"

namespace rhdl::blocks {

class BSDelay : public BlockSim
{
public:
	BSDelay(Queue &q);

	void swtch();
	virtual void set() override {assert(0);}
	void reset() override;

	bool onAfterSwitch() {return on_after_switch_;}

	virtual bool update_internal(char value) override;

protected:
	virtual void resetOnAfterSwitch() = 0;
	virtual void updateOnAfterSwitch(char value) = 0;

	bool on_after_switch_;
};

}

#endif // BSSWITCH_H
