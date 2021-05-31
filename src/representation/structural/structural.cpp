#include <rhdl/construction/constructionexception.h>
#include "structural.h"
#include "representation/netlist/graph_impl.h"
#include "interface/interface.h"
#include "interface/iplaceholder.h"
#include "interface/predicate.h"
#include "interface/visitors/interfacetovertexvisitor.h"
#include "interface/visitors/getcorrespondingsubinterface.h"
#include "interface/visitors/setopenvisitor.h"
#include "interface/visitors/flattenconnectionsvisitor.h"
#include "simulation/hierarchicalsim.h"
#include "entity/entity.h"
#include <iostream>
#include <memory>

using namespace rhdl;

Structural::Structural(const Entity &entity, const Representation *parent,
		const Timing *timing)
	:
		RepresentationBase<Structural>(entity, parent, timing),
		has_flat_connections_(false)
{
	parts_.push_back(&entity);
}

std::unique_ptr<Simulator> Structural::makeSimulator(bool use_behavior) const
{
	return std::make_unique<HierarchicalSim>(*this, use_behavior);
}

PartIdx Structural::add(const Entity *what)
{
	if (entity().isStateless() && !(what -> isStateless()))
		throw ConstructionException(Errorcode::E_STATEFUL_COMPONENT_IN_STATELESS_ENTITY);

	return NetImpl::add(what);
}

/*
 * When an (single) interface is exposed, which already has another part connected, that interface is considered open. (1)
 * The other part's corresponding interface must not contain open subinterfaces. (2)
 */
static void check_ec(const Net::Port &exposed, const Interface *ext_interface, const Net::Port &connected, const Net::Port &other)
{
	auto nondir = Interface::Predicate2::pte_nondir();

	if (exposed >= connected) {
		const Interface &own_sub = ext_interface -> getCorrespondingSubInterface(
				*exposed.second, *connected.second, nondir);

		if (other.second -> is_partially_open())
			throw ConstructionException(Errorcode::E_ALREADY_CONNECTED_TO_OPEN); // (2)

		own_sub.setAllOpen(); // (1)
	}
	else if (connected >= exposed) {
		const Interface &other_sub = other.second -> getCorrespondingSubInterface(
				*connected.second, *exposed.second, nondir);

		if (other_sub.is_partially_open())
			throw ConstructionException(Errorcode::E_ALREADY_CONNECTED_TO_OPEN); // (2)

		ext_interface -> setAllOpen(); // (1)
	}
}

static void check_joining_exposures(const Net::Port &p1, const Interface *ext_super, const Net::Port &p2, const Interface *ext_sub)
{
	const Interface &p1_sub = p1.second -> getCorrespondingSubInterface(
			*ext_super, *ext_sub, Interface::Predicate2::pte_nondir());

	//(2) cannot expose to ext interface, when an open interface was already exposed to it
	if (p1_sub.is_partially_open())
		throw ConstructionException(Errorcode::E_ALREADY_CONNECTED_TO_OPEN); // (2)
	if (p2.second -> is_partially_open())
		throw ConstructionException(Errorcode::E_ALREADY_CONNECTED_TO_OPEN); // (2)
}

static void open_splitting_exposures(const Net::Port &super, const Interface *ext1, const Net::Port &sub, const Interface *ext2)
{
	const Interface &ext1_sub = ext1 -> getCorrespondingSubInterface(
			*super.second, *sub.second, Interface::Predicate2::pte_nondir());

	//(1) set both external interfaces to open
	ext2 -> setAllOpen();
	ext1_sub.setAllOpen();
}


static void check_exposures(const Structural::Connection &assumed_super, const Structural::Connection &assumed_sub)
{
	if (assumed_super.first >= assumed_sub.first) {
		if (assumed_super.second >= assumed_sub.second)
			throw ConstructionException(Errorcode::E_ILLEGAL_RECONNECTION);

		open_splitting_exposures(assumed_super.first, assumed_super.second.second, assumed_sub.first, assumed_sub.second.second);
	}
	else if (assumed_super.second >= assumed_sub.second) {
		check_joining_exposures(assumed_super.first, assumed_super.second.second, assumed_sub.first, assumed_sub.second.second);
	}
}

bool Structural::expose(PartIdx part, const Interface *part_interface, const Interface *ext_interface)
{
	Port to_expose(part, part_interface);
	SetOpenVisitor set_open; //FIXME: delete

	Connection new_exposure(to_expose, Port(0, ext_interface));

	for (const Connection &connection: connections()) {
		if (isExposure(connection)) {
			check_exposures(connection, new_exposure);
			check_exposures(new_exposure, connection);
		}
		else {
			check_ec(to_expose, ext_interface, connection.first, connection.second);
			check_ec(to_expose, ext_interface, connection.second, connection.first);
		}
	}

	connections_.push_back(new_exposure);
	return true;
}



bool Structural::connect(PartIdx from, const Interface *from_interface, PartIdx to, const Interface *to_interface)
{
	Port pfrom(from, from_interface);
	Port pto(to, to_interface);
	Connection new_connection(pfrom, pto);

	for (const Connection &connection : connections()) {
		if (isExposure(connection)) {
			check_ec(connection.first, connection.second.second, pfrom, pto);
			check_ec(connection.first, connection.second.second, pto, pfrom);
		}
		else {
			check(new_connection, connection);
			check(connection, new_connection);
		}
	}

	connections_.push_back ({{from, from_interface}, {to, to_interface}});
	return true;
}

std::string Structural::exposureToString(const Connection &exp) const
{
	assert(isExposure(exp));
	return portToString(exp.first) + " => " + entity().fqn(exp.second.second);
}

const std::vector<Structural::FlatConnection> &Structural::flatConnections() const
{
	makeFlatConnections();
	return flat_connections_;
}

void Structural::makeFlatConnections() const
{
	if (has_flat_connections_)
		return;

	for (auto &connection : connections()) {
		FlattenConnectionsVisitor(flat_connections_,
								  connection.first.first, connection.second.first).
				go_visit(connection.first.second, connection.second.second);
	}

	has_flat_connections_ = true;
}

Structural::operator std::string() const
{
	std::string res = "EComposite\n";
	res += *((NetImpl *) this);
	res += "  exposed:\n";
	for (auto exp : connections_) {
		if (!isExposure(exp))
			continue;

		res += "    ";
		res += exposureToString(exp) + "\n";
	}

	return res;
}

