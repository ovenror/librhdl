/*
 * interfaceindex.cpp
 *
 *  Created on: Jun 1, 2021
 *      Author: ovenror
 */

#include "port.h"
#include "complexport.h"
#include "connection.h"
#include "element.h"
#include "structurebuilder.h"

#include <rhdl/construction/connector.h>
#include "construction/interfacecompatexception.h"

#include "interface/isingle.h"

#include "util/debug.h"

#include <iostream>


namespace rhdl::structural::builder {

Port::Port(std::string name) : Super(std::move(name)) {}

Port::~Port()
{
	invalidateHandles();
}

void Port::connectTo(Port &peer) {
	connect(*this, peer, true);
}

bool Port::isExternal()
{
	return element().isTheStructure();
}

ConnectionPredicate Port::predicate(
		const Port &from, const Port &to, bool directional)
{
	return Element::predicate(from.element(), to.element(), directional);
}

std::unique_ptr<CompatibilityResult> Port::compat(const Port &from,
		const Port &to)
{
	return from.compatTo(to);
}

std::unique_ptr<CompatibilityResult> Port::compatTo(
		const Port &target) const
{
	return compat(target, predicate(*this, target));
}

bool Port::compatible(const Port &from, const Port &to)
{
	return compat(from, to) -> success();
}

bool Port::compatible(const Port &peer, ConnectionPredicate p) const
{
	return compat(peer, p) -> success();
}

std::array<Port*, 2> Port::findCompatibles(
		Port &lhs, Port &rhs, const ConnectionPredicate &p)
{
	auto cresult = lhs.compat(rhs, p);

	if (cresult -> success())
		return {&lhs, &rhs};

	std::array<Port*, 2> found = lhs.findCompatibles(rhs, p);

	assert (!found[0] == !found[1]);

	if (!found[0])
		throw InterfaceCompatException(std::move(cresult));

	return found;
}

void Port::connect(Port &lhs, Port &rhs, bool directional)
{
	auto p = predicate(lhs, rhs, directional);
	auto compats = findCompatibles(lhs, rhs, p);
	assert(compats[0] && compats[1]);
	connectCompat(*compats[0], *compats[1], p);
}

void Port::connectCompat(
		Port &from, Port &to, const ConnectionPredicate &p)
{
	assert (!from.element().isTheStructure() || !to.element().isTheStructure());

	Element::combineBuilders(from.element(), to.element());

	auto &efrom = from.realization(to, p);
	auto &eto = to.realization(efrom, p.reversed());

	efrom.connectCompat(eto);
}

Port &Port::operator[](const std::string &ifaceName)
{
	throw ConstructionException(Errorcode::E_NO_SUCH_INTERFACE);
}

void Port::registerHandle(PortHandle *handle) const
{
	auto result = handles_.insert(handle);
	assert (result.second);
}

void Port::removeHandle(PortHandle *handle) const
{
	auto result = handles_.erase(handle);
	assert (result == 1);
}

void Port::invalidateHandles() const {
	auto handle = handles_.begin();

	while (handle != handles_.end()) {
		(*handle) -> invalidate();
		handle = handles_.erase(handle);
	}
}

std::ostream& operator <<(std::ostream &os, Port &p)
{
	if (p.enclosing())
		os << p.enclosing() -> port();
	else
		os << p.element();

	os << "." << p.name();

	return os;
}

std::array<ExistingPort*, 2> Port::realize(Port &from, Port &to)
{
	auto &rfrom = from.realization(to, predicate(from, to));
	auto &rto = to.realization(from, predicate(to, from));

	return {&rfrom, &rto};
}

ExistingPort& Port::realization(
		BuilderPort &peer, const ConnectionPredicate &p)
{
	assert (0);
	return *static_cast<ExistingPort *>(nullptr);
}

void Port::realizeHandles(ExistingPort &realization) const
{
	for (auto *handle : handles_) {
		handle -> realizePort(realization);
	}
}

std::ostream& operator <<(std::ostream &os, const Port &p)
{
	os << p.element() << "[";
	p.qnameToStream(os);
	os << "]";

	return os;
}

void Port::qnameToStream(std::ostream &os) const
{
	if (enclosing()) {
		enclosing() -> port().qnameToStream(os);
		os << ".";
	}

	os << name();
}

void Port::inheritHandles(Port &&dying)
{
	assert (handles_.empty());
	handles_ = std::move(dying.handles_);
}

} /* namespace rhdl::structural::builder */
