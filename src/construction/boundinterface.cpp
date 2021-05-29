#include <rhdl/construction/boundinterface.h>
#include <rhdl/construction/interfacible.h>
#include "interfacecompatexception.h"
#include "interface/interface.h"
#include "interface/predicate.h"
#include "interface/cresult/compatibilityresult.h"

#include <cassert>

namespace rhdl {

BoundInterface::BoundInterface(const Interface *iface) :
	iface_(*iface)
{
	if (!iface)
		throw ConstructionException(Errorcode::E_NO_SUCH_INTERFACE);
}

void BoundInterface::operator=(const BoundInterface &to) const
{
	//*this >> to;
	try_connect(to, Interface::Predicate2::ptp_nondir());
}

std::pair<const Interface *, const Interface *> BoundInterface::find_connectible_interfaces(const Connectible &to, const Interface::Predicate2 &predicate) const
{
	return {&iface_, to.find_connectible_interface(*this, predicate.reversed())};
}

std::pair<const Interface *, const Interface *> BoundInterface::find_connectible_interfaces(const Interfacible &to, const Interface::Predicate2 &predicate) const
{
	return {&iface_, to.find_connectible_interface(*this, predicate.reversed())};
}

const Interface *BoundInterface::find_connectible_interface(const Connectible &to, const Interface::Predicate2 &predicate) const
{
	Interface::CResult result = iface_.eq_struct(*to.interface(), predicate);

	if (result -> success())
		return &iface_;

	throw InterfaceCompatException(std::move(result));
	return nullptr;
}

}

