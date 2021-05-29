/*
 * exported.cpp
 *
 *  Created on: Apr 4, 2021
 *      Author: ovenror
 */

#include <rhdl/construction/exported.h>
#include <rhdl/construction/interfacible.h>

namespace rhdl {

const Interface* Exported::interface() const
{
	return interfacible().interface();
}

Exported::Exported() {
}

std::pair<const Interface*, const Interface*> Exported::find_connectible_interfaces(
		const Connectible& to, const ConnectionPredicate& predicate) const
{
	return interfacible().find_connectible_interfaces(to, predicate);
}

std::pair<const Interface*, const Interface*> Exported::find_connectible_interfaces(
		const Interfacible& to, const ConnectionPredicate& predicate) const
{
	return interfacible().find_connectible_interfaces(to, predicate);
}

const Interface* Exported::find_connectible_interface(
		const Connectible& to, const ConnectionPredicate& predicate) const
{
	return interfacible().find_connectible_interface(to, predicate);
}

}
