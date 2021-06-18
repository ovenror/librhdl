/*
 * complexport.cpp
 *
 *  Created on: Jul 25, 2021
 *      Author: ovenror
 */

#include <representation/structural/builder/cbuilderportcontainer.h>
#include "portcontainer.h"
#include "existingport.h"
#include "interface/interface.h"

#include <cassert>
#include <unordered_set>

namespace rhdl {
namespace structural {
namespace builder {

PortContainer::PortContainer() {
	// TODO Auto-generated constructor stub

}

PortContainer::~PortContainer() {
	// TODO Auto-generated destructor stub
}

std::array<Port*, 2> PortContainer::findCompatibles(
		Port &peer,	ConnectionPredicate p)
{
	auto peerFound = peer.findCompatible(port(), p.reversed());

	if (peerFound)
		return {&port(), peerFound};

	std::array<Port*, 2> found = {nullptr, nullptr};

	for (auto &sub : enclosed()) {
		std::array<Port*, 2> newFound;

		if (sub.compatible(peer, p))
			newFound = {&sub, &peer};
		else
			newFound = {nullptr, nullptr};

		if (!newFound[0])
			newFound = sub.findCompatibles(peer, p);

		assert (!newFound[0] == !newFound[1]);

		if (!newFound[0])
			continue;

		if (found[0])
			throw ConstructionException(Errorcode::E_FOUND_MULTIPLE_COMPATIBLE_INTERFACES);

		found = newFound;
	}

	return found;
}

Port* PortContainer::findCompatible(Port &peer, ConnectionPredicate p)
{
	Port *found = nullptr;

	for (auto &sub : enclosed()) {
		auto newFound = (sub.compatible(peer, p)) ? &sub : nullptr;

		if (!newFound)
			newFound = sub.findCompatible(peer, p);

		if (!newFound)
			continue;

		if (found)
			throw ConstructionException(Errorcode::E_FOUND_MULTIPLE_COMPATIBLE_INTERFACES);

		found = newFound;
	}

	return found;
}

void PortContainer::adopt(const ExistingPort &p)
{
	p.getAdoptedBy(this);
}

bool PortContainer::Less::operator ()(
		const Port &lhs, const Port &rhs) const
{
	if (lhs.name() != rhs.name())
		return lhs.name() < rhs.name();

	if (lhs.name() == Interface::anon_name)
		return &lhs < &rhs;

	return false;
}

bool PortContainer::Less::operator ()(
		const Port &lhs, const std::string &rhs) const
{
	return lhs.name() < rhs;
}

bool PortContainer::Less::operator ()(
		const std::string &lhs, const Port &rhs) const
{
	return lhs < rhs.name();
}

bool PortContainer::Less::operator ()(
		const std::unique_ptr<ExistingPort> &lhs,
		const Port &rhs) const
{
	return (*this)(*lhs, rhs);
}

bool PortContainer::Less::operator ()(
		const Port &lhs,
		const std::unique_ptr<ExistingPort> &rhs) const
{
	return (*this)(lhs, *rhs);
}

bool PortContainer::Less::operator ()(
		const std::unique_ptr<ExistingPort> &lhs,
		const std::string &rhs) const
{
	return (*this)(*lhs, rhs);
}

bool PortContainer::Less::operator ()(
		const std::string &lhs,
		const std::unique_ptr<ExistingPort> &rhs) const
{
	return (*this)(lhs, *rhs);
}

bool PortContainer::Less::operator ()(
		const std::unique_ptr<ExistingPort> &lhs,
		const std::unique_ptr<ExistingPort> &rhs) const
{
	return (*this)(*lhs, *rhs);
}

const Port& PortContainer::port() const
{
	return const_cast<PortContainer *>(this) -> port();
}

} /* namespace builder */
} /* namespace structural */
} /* namespace rhdl */

