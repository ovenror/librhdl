/*
 * connectionbuilder.cpp
 *
 *  Created on: May 28, 2022
 *      Author: ovenror
 */

#include "connectionsbuilder.h"
#include "simpleconnection.h"
#include "complexconnection.h"
#include "element.h"

#include "../connection.h"

namespace rhdl {
namespace structural {
namespace builder {

ConnectionsBuilder::ConnectionsBuilder(
		const std::map<Element*, ElementIdx> &elementMap)
	: elementMap_(elementMap)
{}

ConnectionsBuilder::~ConnectionsBuilder() {}

void ConnectionsBuilder::build(const SimpleConnection &c)
{
	std::vector<structural::Port> ports;

	for (auto &port : c) {
		ports.emplace_back(elementMap_.at(&port.element()), port.iface());
	}

	result_.emplace_back(std::move(ports));
}

void ConnectionsBuilder::build(const ComplexConnection &c)
{
	for (auto *child : c.children())
		queue_.push(*child);
}

std::set<const Connection *>
	ConnectionsBuilder::findTopConnections() const
{
	std::set<const Connection *> result;

	for (const auto [e, eidx] : elementMap_) {
		auto *topReal = e -> topPort().realization();

		if (!topReal)
			continue;

		result.insert(&topReal -> connection());
	}

	return result;
}

std::vector<structural::Connection>	ConnectionsBuilder::build()
{
	for (auto connection : findTopConnections())
		queue_.push(*connection);

	std::set<const Connection *> done;

	while (!queue_.empty()) {
		auto &connection = queue_.front();
		queue_.pop();

		if (done.find(&connection.get()) != done.end())
			continue;

		connection.get().build(*this);
		done.insert(&connection.get());
	}

	return result_;
}

} /* namespace builder */
} /* namespace structural */
} /* namespace rhdl */
