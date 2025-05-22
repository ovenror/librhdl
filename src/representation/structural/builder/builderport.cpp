/*
 * builderport.cpp
 *
 *  Created on: Jul 24, 2021
 *      Author: ovenror
 */

#include "cbuilderportcontainer.h"
#include "cpeerdirection.h"
#include "portscreator.h"
#include "builderport.h"
#include "complexport.h"
#include "element.h"
#include "newentitystructure.h"
#include "cbuildersimple.h"
#include "construction/interfacecompatexception.h"

#include "interface/interface.h"
#include "interface/cresult/ctrue.h"

namespace rhdl {
namespace structural {
namespace builder {

BuilderPort::BuilderPort(
		NewEntityStructure &structure, BuilderPort *enclosing,
		std::string name)
	: Port(name), structure_(structure), enclosing_(enclosing)
{
	Port::setDictionary(dictionary::DereferencingDictionaryAdapter<decltype(enclosed_), const CObject>(enclosed_));
	PortContainer::setDictionary(dictionary::DereferencingDictionaryAdapter<decltype(enclosed_)>(enclosed_));

	c_ptr() -> iface = &c_iface_;

	c_iface_.type = enclosing_ ? RHDL_UNSPECIFIED : RHDL_COMPOSITE;
	c_iface_.composite.interfaces = c_strings().data();
}

BuilderPort::~BuilderPort()
{
	for (auto &port : enclosed_)
		port -> invalidateHandles();
}

const Interface& BuilderPort::iface() const
{
	assert(0);
	return *static_cast<Interface *>(nullptr);
}

BuilderPort& BuilderPort::encloseNew(const std::string &ifaceName)
{
	auto newPort =std::make_unique<BuilderPort>(structure_, this, ifaceName);
	auto &result = *newPort;

	add(enclosed_, std::move(newPort));

	c_iface_.type = RHDL_COMPOSITE;
	c_iface_.composite.interfaces = c_strings().data();

	return result;
}

void BuilderPort::removeLastEnclosed(const BuilderPort &port)
{
	assert (&port == enclosed_.back().get());

	enclosed_.erase(port.name());

	if (enclosing_ && enclosed_.empty())
		c_iface_.type = RHDL_UNSPECIFIED;
}

Port& BuilderPort::operator [](const std::string &ifaceName)
{
	if (enclosed_.contains(ifaceName))
		return *enclosed_.at(ifaceName);

	return encloseNew(ifaceName);
}

std::unique_ptr<CompatibilityResult> BuilderPort::compat(
		const Port &peer, ConnectionPredicate p) const
{
	return peer.compat(*this, p.reversed());
}

std::unique_ptr<CompatibilityResult> BuilderPort::compat(
		const ExistingPort &peer, ConnectionPredicate p) const
{
	return std::make_unique<CBuilderSimple>(*this, peer, p);
}

std::unique_ptr<CompatibilityResult> BuilderPort::compat(
		const ComplexPort &peer, ConnectionPredicate p) const
{
	return compat(static_cast<const PortContainer &>(peer), p);
}

std::unique_ptr<CompatibilityResult> BuilderPort::compat(
		const BuilderPort &peer, ConnectionPredicate p) const
{
	assert (0);
	return compat(static_cast<const PortContainer &>(peer), p);
}

std::unique_ptr<CompatibilityResult> BuilderPort::compat(
		const PortContainer &peer, ConnectionPredicate p) const
{
	return std::make_unique<CBuilderPortContainer>(*this, peer, p);
}

std::array<Port*, 2> BuilderPort::findCompatibles(
		Port &peer,	ConnectionPredicate p)
{
	auto result = peer.findCompatibles(*this, p.reversed());
	std::swap(result[0], result[1]);
	return result;
}

std::array<Port*, 2> BuilderPort::findCompatibles(
		BuilderPort &peer, ConnectionPredicate p)
{
	assert (0);
	return {nullptr, nullptr};
}

std::array<Port*, 2> BuilderPort::findCompatibles(
		ExistingPort &peer,	ConnectionPredicate p)
{
	auto found = PortContainer::findCompatibles(peer, p);

	assert (!found[0] == !found[1]);

	/* FIXME: Maybe avoid this check (not only here), by deriving a
	 * TopBuilderPort from BuilderPort.
	 */
	if (found[0] && found[0] -> enclosing())
		return found;

	/* FIXME: Avoid this by deriving an AnonymousBuilderPort
	 * from BuilderPort that cannot create new AnonymousBuilderPorts
	 */
	if (name() == Interface::anon_name) {
		throw InterfaceCompatException(compat(peer, p));
	}

	BuilderPort anon = BuilderPort(structure_, this, Interface::anon_name);

	if (anon.compatible(peer, p))
		found = {&peer, &anon};
	else
		found = peer.findCompatibles(anon, p.reversed());

	assert (!found[0] == !found[1]);
	assert (!found[1] || found[1] == &anon);

	if (!found[0])
		return {nullptr, nullptr};

	auto *peerFound = found[0];
	auto &namedEnclosed = encloseNew(peerFound -> name());

	return {&namedEnclosed, peerFound};
}

std::array<Port*, 2> BuilderPort::findCompatibles(
		ComplexPort &peer, ConnectionPredicate p)
{
	return findCompatibles(static_cast<ExistingPort &>(peer), p);
}

Port* BuilderPort::findCompatible(Port &peer, ConnectionPredicate p)
{
	auto found = PortContainer::findCompatible(peer, p);

	if (found)
		return found;

	if (!CPeerDirection::compatible(peer, p))
		return nullptr;

	return &encloseNew(peer.name());
}

std::unique_ptr<ExistingPort> BuilderPort::constructExistingPort(
		ExistingPort &peer, ConnectionPredicate predicate)
{
	assert (predicate.samedir_);

	auto *iface = peer.iface().clone(name());
	assert (iface -> compatTo(peer.iface(), predicate));

	return PortsCreator(element()).create(*iface, std::move(c_));
}

void BuilderPort::replaceEnclosedBuilder(
		BuilderPort &builder, std::unique_ptr<ExistingPort> &&newPort)
{
	adopt(*newPort);
	encloseDirection(newPort -> direction());

	enclosed_.replace(std::move(newPort));
}

ExistingPort &BuilderPort::realization(
		ExistingPort &peer,	const ConnectionPredicate &p)
{
	return replace(constructExistingPort(peer, p));
}

ExistingPort& BuilderPort::realization(
		Port &peer,	const ConnectionPredicate &p)
{
	return realization(peer.realization(*this, p.reversed()), p);
}

ExistingPort* BuilderPort::realization()
{
	auto components = ifaces();

	if (components.empty())
		return nullptr;

	auto iface = new IComposite(name(), components);

	auto real = constructComplexPort(*iface);
	realizeHandles(*real);

	if (enclosing_)
		return &replace(std::move(real));
	else
		return &replaceTop(std::move(real));
}

std::unique_ptr<ComplexPort> BuilderPort::constructComplexPort(const IComposite &itop)
{
	dictionary::FCFSDictionary<std::unique_ptr<ExistingPort>> existingEnclosed;

	for (auto port = enclosed_.begin(); port != enclosed_.end();) {
		ExistingPort *real = (*port) -> realization();

		// all enclosed ports should have been realized at this point
		assert (real == port -> get());

		enclosed_.erase((*port) -> name()).release();
		existingEnclosed.add(std::unique_ptr<ExistingPort>(real));
		port = enclosed_.begin();
	}

	auto real = std::make_unique<ComplexPort>(
			structure_, itop, std::move(existingEnclosed),
			std::move(c_), &name());
	real -> autoconsolidate();
	return std::move(real);
}

ExistingPort &BuilderPort::replace(std::unique_ptr<ExistingPort> &&realization)
{
	assert (enclosing_);

	replace_common(*realization);

	auto &result = *realization;
	enclosing_ -> replaceEnclosedBuilder(*this, std::move(realization));
	return result;
}

ComplexPort &BuilderPort::replaceTop(std::unique_ptr<ComplexPort> &&realization)
{
	assert (!enclosing_);

	replace_common(*realization);

	auto &result = *realization;
	element().replaceTopBuilder(std::move(realization));
	return result;
}

void BuilderPort::replace_common(ExistingPort &realization)
{
	assert (Port::interchangeable(*this, realization));

	realization.inheritHandles(std::move(*this));
}

std::vector<const Interface*> BuilderPort::ifaces()
{
	std::vector<const Interface*> result;

	for (auto &p : enclosed_) {
		auto real = p -> realization();

		if (real)
			result.emplace_back(&real -> iface());
	}

	return result;
}

Element& BuilderPort::element() const
{
	return structure_;
}

void BuilderPort::encloseDirection(CompositeDirection newDir) {
	auto old = direction_;
	direction_ |= newDir;

	if (direction_ != old && enclosing_)
		enclosing_ -> encloseDirection(direction_);
}

} /* namespace builder */
} /* namespace structural */
} /* namespace rhdl */
