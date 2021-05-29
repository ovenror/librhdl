#ifndef INVERTERSIM_H
#define INVERTERSIM_H

#include "simulator.h"

namespace rhdl {

class EInverter;

class InverterSim : public Simulator
{
public:
	InverterSim(const EInverter &inverter);

	virtual bool get(const ISingle *iface) const override;
	void processInput() override;
	void internalStep() override;

private:
	bool inputstate_, state_;
	const ISingle *in_, *out_;
};

}

#endif // INVERTERSIM_H
