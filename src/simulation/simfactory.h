#ifndef SIMFACTORY_H
#define SIMFACTORY_H

#include <memory>

namespace rhdl {

class Interface;
class Simulator;

class SimFactory
{
public:
	virtual ~SimFactory() {}
	virtual std::unique_ptr<Simulator> make(const Interface &iface) = 0;
};

}


#endif // SIMFACTORY_H
