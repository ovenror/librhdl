/*
 * connection.cpp
 *
 *  Created on: Jun 14, 2021
 *      Author: ovenror
 */

#include "connection.h"
#include "port.h"
#include "structurebuilder.h"
#include "complexconnection.h"

#include "interface/icomposite.h"

#include "util/pointingpartitionclass.h"

#include <array>
#include <memory>
#include <queue>
#include <vector>

namespace rhdl::structural::builder {

void Connection::addParent(ComplexConnection &c)
{
	parents_.insert(&c);
}

void Connection::adoptRelativesFrom(Connection &c)
{
	copyRelatives(c.parents_, parents_);

	for (auto *parent: c.parents_)
		parent -> replaceChild(c, *this);
}

void Connection::replaceParent(ComplexConnection &old, ComplexConnection &n)
{
	parents_.erase(&old);
	parents_.insert(&n);
}

} /* namespace rhdl::structural::builder */
