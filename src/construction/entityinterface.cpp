#include <rhdl/construction/boundinterface.h>
#include <rhdl/construction/entityinterface.h>
#include "construction/entityhandle.h"
#include "entity/entity.h"


namespace rhdl {

EntityInterface::EntityInterface(const EntityHandle &ehandle, const Interface *interface) :
	Super(interface), ehandle_(ehandle)
{
}

void EntityInterface::operator=(const PartInterface &other) const
{
	//BoundInterface::operator= (other);
}

EntityInterface EntityInterface::operator[](const std::string &iname) const
{
	return EntityInterface(ehandle_, iface_[iname]);
}

const Interfacible &EntityInterface::owner() const
{
	return ehandle_;
}

}
