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

#include "interface/interface.h"
#include "interface/cresult/ctrue.h"

namespace rhdl {
namespace structural {
namespace builder {

BuilderPort::BuilderPort(
		NewEntityStructure &structure, BuilderPort *enclosing,
		size_t index, std::string name)
	: structure_(structure), enclosing_(enclosing),
	  orderIndex_(index), name_(name), c_(*this)
{
	auto &c = *c_ptr(*this);

	c.type = enclosing_ ? RHDL_UNSPECIFIED : RHDL_COMPOSITE;
	c.composite.interfaces = c_strings_.data();
}

BuilderPort::~BuilderPort()
{
	for (auto &port : enclosed_)
		port.invalidateHandles();
}

const Interface& BuilderPort::iface() const
{
	assert(0);
	return *static_cast<Interface *>(nullptr);
}

BuilderPort& BuilderPort::encloseNew(const std::string &ifaceName)
{
	assertLenghts();

	auto newPort =std::make_unique<BuilderPort>(
		structure_, this, enclosedOrder_.size(), ifaceName);
	auto &result = *newPort;

	auto [iterator, inserted] = enclosed_.insert(std::move(newPort));
	assert (inserted);
	enclosedOrder_.emplace_back(&result);

	auto &c = *c_ptr(*this);

	c.type = RHDL_COMPOSITE;
	c_strings_.back() = result.name_.data();
	c_strings_.push_back(nullptr);
	c.composite.interfaces = c_strings_.data();

	assertLenghts();

	return result;
}

void BuilderPort::removeLastEnclosed(const BuilderPort &port)
{
	assertLenghts();

	assert (&port == enclosedOrder_.back());

	auto iterator = enclosed_.find(port);
	assert (iterator != enclosed_.end());
	assert (&*iterator == &port);

	auto &c = *c_ptr(*this);

	c_strings_.pop_back();
	assert (c_strings_.back() = port.name_.data());
	c_strings_.back() = nullptr;
	if (enclosing_ && enclosed_.empty())
		c.type = RHDL_UNSPECIFIED;

	enclosedOrder_.pop_back();
	enclosed_.erase(iterator);

	assertLenghts();
}

void BuilderPort::assertLenghts() const
{
	assert (c_strings_.size() == enclosed_.size() + 1);
	assert (enclosedOrder_.size() == enclosed_.size());
}


Port& BuilderPort::operator [](const std::string &ifaceName)
{
	auto port = enclosed_.find(ifaceName);
	if (port != enclosed_.end())
		return *port;

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

	if (found[0])
		return found;

	auto &anonEnclosed = encloseNew();

	if (anonEnclosed.compatible(peer, p))
		found = {&peer, &anonEnclosed};
	else
		found = peer.findCompatibles(anonEnclosed, p.reversed());

	assert (!found[0] == !found[1]);
	assert (!found[1] || found[1] == &anonEnclosed);

	removeLastEnclosed(anonEnclosed);

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

	auto *iface = peer.iface().clone(name_);
	assert (iface -> compatTo(peer.iface(), predicate));

	return PortsCreator(element()).create(*iface);
}

void BuilderPort::replaceEnclosedBuilder(
		BuilderPort &builder, std::unique_ptr<ExistingPort> &&newPort)
{
	assertLenghts();

	adopt(*newPort);
	encloseDirection(newPort -> direction());

	c_strings_[builder.orderIndex_] = newPort -> name().data();
	enclosedOrder_[builder.orderIndex_] = newPort.get();

	auto erase_result = enclosed_.erase(builder);
	assert (erase_result == 1);

	auto insert_result = enclosed_.insert(std::move(newPort));
	assert (insert_result.second);

	assertLenghts();
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

	auto iface = new IComposite(name_, components);

	auto real = constructComplexPort(*iface);
	realizeHandles(*real);

	if (enclosing_)
		return &replace(std::move(real));
	else
		return &replaceTop(std::move(real));
}

std::unique_ptr<ComplexPort> BuilderPort::constructComplexPort(const IComposite &itop)
{
	pc::Poly<std::set<std::unique_ptr<ExistingPort>, Less>, Port> existingEnclosed;

	for (auto port = enclosed_.begin(); port != enclosed_.end();) {
		ExistingPort *real = port -> realization();

		// all enclosed ports should have been realized at this point
		assert (real == &*port);

		enclosed_.extract(port).value().release();
		existingEnclosed.emplace(std::unique_ptr<ExistingPort>(real));
		port = enclosed_.begin();
	}

	auto real = std::make_unique<ComplexPort>(structure_, itop, std::move(existingEnclosed));
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

	for (auto p : enclosedOrder_) {
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

const rhdl_iface_struct* BuilderPort::c_ptr_iface() const
{
	return c_ptr(*this);
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
