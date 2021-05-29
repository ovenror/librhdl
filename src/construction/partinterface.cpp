#include <rhdl/construction/partinterface.h>
#include "parthandle.h"

#include "interface/interface.h"

#include <cassert>

namespace rhdl {

PartInterface::PartInterface(const PartHandle &part, const Interface *interface) :
	BoundInterface(interface), part_(part)
{
	
}

PartInterface PartInterface::operator[](const std::string &iname) const
{
	return PartInterface(part_, iface_[iname]);
}

const Interfacible &PartInterface::owner() const
{
	return part_;
}


}
