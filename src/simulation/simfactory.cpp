#include "simfactory.h"
#include "entity/entity.h"

namespace rhdl {

SimFactory::SimFactory(const Entity &e)
	: iface_(&e.interface())
{

}
}

