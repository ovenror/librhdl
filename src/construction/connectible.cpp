#include <representation/structural/builder/port.h>
#include <rhdl/construction/connectible.h>
#include <rhdl/construction/constructionexception.h>

#include "interface/interface.h"
#include "interface/predicate.h"

#include <cassert>
#include <iostream>

namespace rhdl {

using structural::builder::Port;

static void connect(Port &from, Port &to, bool directional = true)
{
	Port::connect(from, to, directional);
}

Connectible::Connectible() {
}

const Connectible &Connectible::operator>>(const Connectible &to) const
{
	connect(port(), to.port(), true);
	return const_cast<Connectible &>(to);
}

const Connectible &Connectible::operator<<(const Connectible &from) const
{
	connect(from.port(), port(), true);
	return const_cast<Connectible &>(from);
}

Connectible& Connectible::operator=(const Connectible &peer)
{
	connect(port(), peer.port(), false);
	return const_cast<Connectible &>(peer);
}

}
