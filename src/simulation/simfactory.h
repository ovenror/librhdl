#ifndef SIMFACTORY_H
#define SIMFACTORY_H

#include <memory>
#include "simulator.h"

namespace rhdl {

class Entity;
class Interface;

class SimFactory
{
public:
	SimFactory(const Entity &e);

	virtual std::unique_ptr<Simulator> make() = 0;

protected:
	const Interface *iface_;
};

}


#endif // SIMFACTORY_H
