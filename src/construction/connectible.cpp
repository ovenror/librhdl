#include <rhdl/construction/connectible.h>
#include <rhdl/construction/constructionexception.h>
#include <rhdl/construction/interfacible.h>
#include "interface/interface.h"
#include "interface/predicate.h"

#include "representation/structural/netimpl.h"

#include <cassert>
#include <iostream>

namespace rhdl {

Connectible::Connectible()
{
}

const Connectible &Connectible::operator>>(const Connectible &to) const
{
	Errorcode ec;
	if ((ec = try_connect(to, Interface::Predicate2::ptp())) != Errorcode::E_NO_ERROR)
		throw ConstructionException(ec);

	return to;
}

const Connectible &Connectible::operator<<(const Connectible &from) const
{
	from >> *this;
	return from;
}

Errorcode Connectible::try_connect(const Connectible &to, const Interface::Predicate2 &predicate) const
{
	//std::cerr << ">>> Connecting " << typeid(*this).name() << " to " << typeid(to).name() << std::endl;

	std::pair<const Interface*, const Interface*> interfaces;
	interfaces = find_connectible_interfaces(to, predicate);

	if (!interfaces.first || !interfaces.second)
		return Errorcode::E_COMPATIBLE_INTERFACES_NOT_FOUND;

	if (!to.interfacible().get_connected(interfaces.second, interfacible(), interfaces.first))
		return Errorcode::E_UNKNOWN_CONNECTION_FAILURE;


	return Errorcode::E_NO_ERROR;
}

}
