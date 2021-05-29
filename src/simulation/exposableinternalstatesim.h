#ifndef EXPOSABLEINTERNALSTATESIM_H
#define EXPOSABLEINTERNALSTATESIM_H

#include "simulator.h"

namespace rhdl {

class ExposableInternalStateSim : public Simulator
{
public:
	ExposableInternalStateSim();

protected:
	void processInput() override;

	virtual void setInternal(const ISingle *iface) = 0;
};


}
#endif // EXPOSABLEINTERNALSTATESIM_H
