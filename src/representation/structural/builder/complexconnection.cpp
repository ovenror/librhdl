/*
 * complexconnection.cpp
 *
 *  Created on: May 27, 2022
 *      Author: ovenror
 */

#include "complexconnection.h"
#include "connectionsbuilder.h"

namespace rhdl {
namespace structural {
namespace builder {

void ComplexConnection::setInitialChildren(std::set<Connection*> children)
{
	assert (children_.empty());
	children_ = std::move(children);
}

void ComplexConnection::adoptRelativesFrom(Connection &c)
{
	c.getRelativesAdoptedBy(*this);
}

void ComplexConnection::adoptRelativesFrom(ComplexConnection &c)
{
	Connection::adoptRelativesFrom(c);

	copyRelatives(c.children_, children_);

	for (auto *child: c.children_)
		child -> replaceParent(c, *this);
}

void ComplexConnection::getRelativesAdoptedBy(ComplexConnection &c)
{
	c.adoptRelativesFrom(*this);
}

void ComplexConnection::build(ConnectionsBuilder &b) const
{
	b.build(*this);
}

void ComplexConnection::replaceChild(Connection &old, Connection &n)
{
	children_.erase(&old);
	children_.insert(&n);
}

} /* namespace builder */
} /* namespace structural */
} /* namespace rhdl */
