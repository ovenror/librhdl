#ifndef INVERTER_H
#define INVERTER_H

#include "entity.h"
#include "timing.h"

namespace rhdl {

class Timing;

class EInverter : public Entity
{
public:
	EInverter();

	operator std::string() const;

private:
	void makeNetlist(const Timing &timing);
	void makeTimedBehavior(const Timing &timing);
	void makeFunctionalBehavior();
};

}

#endif // INVERTER_H
