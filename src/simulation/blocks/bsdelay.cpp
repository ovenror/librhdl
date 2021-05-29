#include "bsdelay.h"

namespace rhdl {

BSDelay::BSDelay(Queue &q) : BlockSim(q)
{}

void BSDelay::swtch()
{
	if (on_after_switch_) {
		value_ = redstone::maxPower;
		queue_.push(this);
	}
	else {
		value_ = 0;
	}

	//std::cerr << *this << " switched " << (int) value_ << std::endl;
}

void BSDelay::reset()
{
	if (value_)
		queue_.push(this);

	resetOnAfterSwitch();
}

bool BSDelay::update_internal(char value)
{
	updateOnAfterSwitch(value);
	return false;
}


}
