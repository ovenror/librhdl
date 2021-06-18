/*
 * existingport.cpp
 *
 *  Created on: Jul 25, 2021
 *      Author: ovenror
 */

#include <rhdl/construction/porthandle.h>

#include "existingport.h"
#include "complexport.h"
#include "builderport.h"
#include "element.h"
#include "structurebuilder.h"
#include "connection.h"

#include "interface/interface.h"

namespace rhdl {
namespace structural {
namespace builder {

ExistingPort::ExistingPort(Element &element, const Interface &iface)
	: element_(element)
{}

ExistingPort::~ExistingPort() {}

bool Port::interchangeable(const Port &lhs, const Port &rhs)
{
	return lhs.compat(rhs, ConnectionPredicate::equal()) -> success();
}

std::unique_ptr<CompatibilityResult> ExistingPort::compat(
		const Port &peer, ConnectionPredicate p) const
{
	return peer.compat(*this, p.reversed());
}

std::unique_ptr<CompatibilityResult> ExistingPort::compat(
		const ExistingPort &peer, ConnectionPredicate p) const
{
	return iface().compatTo(peer.iface(), p);
}

std::unique_ptr<CompatibilityResult> ExistingPort::compat(
		const BuilderPort &peer, ConnectionPredicate p) const
{
	return peer.compat(*this, p.reversed());
}

std::unique_ptr<CompatibilityResult> ExistingPort::compat(
		const ComplexPort &peer, ConnectionPredicate p) const
{
	return compat(static_cast<const ExistingPort &>(peer), p);
}

std::array<Port*, 2> ExistingPort::findCompatibles(
		Port &peer,	ConnectionPredicate p)
{
	auto result = peer.findCompatibles(*this, p.reversed());
	std::swap(result[0], result[1]);
	return result;
}

std::array<Port*, 2> ExistingPort::findCompatibles(
		ExistingPort &peer,	ConnectionPredicate p)
{
	return {nullptr, nullptr};
}

std::array<Port*, 2> ExistingPort::findCompatibles(
		ComplexPort &peer,ConnectionPredicate p)
{
	return findCompatibles(static_cast<Port &>(peer), p);
}

std::array<Port*, 2> ExistingPort::findCompatibles(
		BuilderPort &peer, ConnectionPredicate p)
{
	return findCompatibles(static_cast<Port &>(peer), p);
}

Port* ExistingPort::findCompatible(Port &peer, ConnectionPredicate p)
{
	return nullptr;
}

const std::string& ExistingPort::name() const
{
	return iface().name();
}

ExistingPort& ExistingPort::realization(Port &peer,
		const ConnectionPredicate &p)
{
	assert(compatible(peer, p));
	return *this;
}

ExistingPort& ExistingPort::realization(ExistingPort &peer,
		const ConnectionPredicate &p)
{
	return realization(static_cast<Port &>(peer), p);
}

ExistingPort& ExistingPort::realization(BuilderPort &peer,
		const ConnectionPredicate &p)
{
	return realization(static_cast<Port &>(peer), p);
}

bool Port::operator <(const Port &p) const
{
	if (name() != p.name())
		return name() < p.name();

	if (name() == Interface::anon_name)
		return this < &p;

	return false;
}

CompositeDirection ExistingPort::direction() const
{
	return iface().compositeDirection();
}

const rhdl_iface_struct* ExistingPort::c_ptr_iface() const
{
	return c_ptr(iface());
}

void ExistingPort::realizeHandles()
{
	for (auto *handle : handles())
		handle -> realizePort(*this);
}

void ExistingPort::inheritHandles(BuilderPort &&p)
{
	Port::inheritHandles(std::move(p));
	realizeHandles();
}

} /* namespace builder */
} /* namespace structural */
} /* namespace rhdl */
