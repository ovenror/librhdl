/*
 * complexinterfaceindex.cpp
 *
 *  Created on: Jun 1, 2021
 *      Author: ovenror
 */

#include "complexport.h"
#include "builderport.h"
#include "portscreator.h"
#include "complexconnection.h"

#include "interface/icomposite.h"

#include <algorithm>

namespace rhdl::structural::builder {

ComplexPort::ComplexPort(
		Element &element, const IComposite &iface,
		Enclosed &&enclosed, const std::string *name)
		: ExistingPortBase<ComplexPort>(element, iface, name), iface_(iface),
		  enclosed_(std::move(enclosed))
{
	constructorCommon();
}

ComplexPort::ComplexPort(Element &element, const IComposite &iface,
		Enclosed &&enclosed, std::unique_ptr<Wrapper<Port>> c,
		const std::string *name)
		: ExistingPortBase<ComplexPort>(element, iface, std::move(c), name),
		  iface_(iface), enclosed_(std::move(enclosed))
{
	constructorCommon();
}


void ComplexPort::constructorCommon() {
	adoptEnclosed();

	Port::setDictionary(dictionary::DereferencingDictionaryAdapter<decltype(enclosed_), const CObject>(enclosed_));
	PortContainer::setDictionary(dictionary::DereferencingDictionaryAdapter<decltype(enclosed_), Port>(enclosed_));

	auto *connection = new ComplexConnection(*this);
	establishConnectionRelations(*connection);

	c_ptr() -> iface = iface_.c_ptr();
}

ComplexPort::~ComplexPort()
{
	for (auto &port : enclosed_)
		port -> invalidateHandles();
}

template <bool (*f)(ExistingPort &, ExistingPort &)>
bool ComplexPort::componentWise(const ComplexPort &peer) const
{
	auto helper =
		[&](const Interface &subIface, const Interface &peerSubIface)
		{
			auto &subPort = get(subIface);
			auto &peerSubPort = get(peerSubIface);

			return f(get(subIface), get(peerSubIface));
		};

	return iface_.componentWise(peer.iface_, helper);
}

bool ComplexPort::componentWise(
		const std::vector<const ComplexPort*> &ports,
		const std::function<bool(const std::vector<const ExistingPort *>)> &f)
{
	std::vector<const IComposite *> interfaces;

	auto size = ports.size();

	interfaces.reserve(size);

	for (const auto *port : ports)
		interfaces.push_back(&port -> iface_);

	auto helper =
		[&](const std::vector<const Interface *> &subInterfaces)
		{
			std::vector<const ExistingPort *> subPorts;
			subPorts.reserve(size);

			auto subInterface = subInterfaces.begin();
			auto port = ports.begin();

			while (subInterface != subInterfaces.end()) {
				assert (port != ports.end());
				subPorts.push_back(&(*port) -> get(**subInterface));
			}

			return f(subPorts);
		};

	return IComposite::componentWise(interfaces, helper);
}

std::array<Port*, 2> ComplexPort::findCompatibles(
		Port &peer,	ConnectionPredicate p)
{
	auto result = peer.findCompatibles(*this, p.reversed());
	std::swap(result[0], result[1]);
	return result;
}

std::array<Port*, 2> ComplexPort::findCompatibles(
		ExistingPort &peer, ConnectionPredicate p)
{
	return PortContainer::findCompatibles(peer, p);
}

std::array<Port*, 2> ComplexPort::findCompatibles(
		ComplexPort &peer, ConnectionPredicate p)
{
	return findCompatibles(static_cast<ExistingPort &>(peer), p);
}

std::array<Port*, 2> ComplexPort::findCompatibles(
		BuilderPort &peer, ConnectionPredicate p)
{
	return findCompatibles(static_cast<Port &>(peer), p);
}

Port* ComplexPort::findCompatible(Port &peer, ConnectionPredicate p)
{
	return PortContainer::findCompatible(peer, p);
}

Port& ComplexPort::operator[](const std::string &ifaceName)
{
	if (!enclosed_.contains(ifaceName))
		throw ConstructionException(Errorcode::E_NO_SUCH_INTERFACE);

	return *enclosed_.at(ifaceName);
}

std::unique_ptr<CompatibilityResult> ComplexPort::compat(
		const Port &peer, ConnectionPredicate p) const
{
	return peer.compat(*this, p.reversed());
}

std::unique_ptr<CompatibilityResult> ComplexPort::compat(
		const BuilderPort &peer, ConnectionPredicate p) const
{
	return peer.compat(*this, p.reversed());
}

const Interface& ComplexPort::iface() const
{
	return iface_;
}

void ComplexPort::adoptEnclosed(){
	for (auto &p: enclosed_)
		adopt(*p);
}

void ComplexPort::connectCompat(ExistingPort &peer)
{
	peer.connectCompat(*this);
}

static bool componentConnect(ExistingPort &lhs, ExistingPort &rhs)
{
	lhs.connectCompat(rhs);
	return true;
}

void ComplexPort::connectCompat(ComplexPort &peer)
{
	ExistingPortBase<ComplexPort>::connectCompat(peer);
	componentWise<&componentConnect>(peer);
}

ExistingPort& ComplexPort::get(const Interface &iface) const
{
	if (iface.name() == Interface::anon_name)
		throw ConstructionException(Errorcode::E_CANNOT_GET_ANONYMOUS_INTERFACE);

	return *enclosed_.at(iface.name());
}

void ComplexPort::establishConnectionRelations(ComplexConnection &connection)
{
	std::set<Connection *> connectionChildren;

	for (auto &e : enclosed_) {
		auto &childConnection = e -> connection();
		connectionChildren.insert(&childConnection);
		childConnection.addParent(connection);
	}

	connection.setInitialChildren(std::move(connectionChildren));
}

} /* namespace rhdl::structural::builder */
