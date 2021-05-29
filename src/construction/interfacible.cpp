#include <rhdl/construction/boundinterface.h>
#include <rhdl/construction/interfacible.h>
#include "interface/interface.h"
#include "interface/predicate.h"

#include <string>
#include <iostream>
#include <tuple>

namespace rhdl {

Interfacible::Interfacible()
{
}

std::pair<const Interface *, const Interface *> Interfacible::find_connectible_interfaces(const Connectible &to, const Interface::Predicate2 &predicate) const
{
	//std::cerr << "I>>C: reverse" << std::endl;
	std::pair<const Interface *, const Interface *> result;
	std::tie(result.second, result.first) = to.find_connectible_interfaces(*this, predicate.reversed());
	return result;
}

std::pair<const Interface *, const Interface *> Interfacible::find_connectible_interfaces(const Interfacible &to, const Interface::Predicate2 &predicate) const
{
	//std::cerr << "I>>I: delegate to interface" << std::endl;
	return interface() -> find_connectibles(to.interface(), predicate);
}

const Interface *Interfacible::find_connectible_interface(const Connectible &to, const Interface::Predicate2 &predicate) const
{
	//std::cerr << "R(I>>C): delegate to interface" << std::endl;
	return interface() -> find_connectible(to.interface(), predicate);
}

}
