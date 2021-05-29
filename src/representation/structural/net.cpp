#include <rhdl/construction/constructionexception.h>
#include "net.h"
#include "temporarynet.h"
#include "interface/visitors/getcorrespondingsubinterface.h"
#include "entity/entity.h"
#include <cassert>

namespace rhdl {

Net::Net()
{

}

Net::~Net()
{

}

#if 0
bool Net::connect(Net::Port from, Net::Port to)
{
	connect(from.first, from.second, to.first, to.second)
}
#endif

bool operator>=(const Net::Port &super, const Net::Port &sub)
{
	return (super.first == sub.first) && (*super.second >= *sub.second);
}

bool operator>=(const Net::Connection &super, const Net::Connection &sub)
{
	return (super.first >= sub.first) && (super.second >= sub.second);
}

void Net::absorb(Net &victim)
{
	if (this == &victim)
		return;

	absorb_dispatch(victim);
}

void Net::absorb_dispatch(Net &victim)
{
	victim.meld_into(*this);
}

void Net::meld_into(Net &common)
{
	std::ignore = common;
	assert(0);
}

void Net::meld_into(TemporaryNet &common)
{
	common.meld_into(*this);
}

void Net::insert_connection_into(const Connection &connection, Net &into)
{
	into.insert_connection(connection);
}


std::string Net::portToString(const Net::Port &port) const
{
	std::string res = "(";
	res += std::to_string(port.first) + ")[" + parts()[port.first] -> fqn(port.second) + "]";
	return res;
}

std::string Net::connectionToString(const Net::Connection &connection) const
{
	return portToString(connection.first) + " => " + portToString(connection.second);
}

Net::operator std::string() const
{
	std::string res = "  parts:\n";

	for (size_t idx=0; idx < parts().size(); ++idx) {
		res += "    (";
		res += std::to_string(idx) + "): " + parts()[idx] -> name() + "\n";
	}

	res += "  connections:\n";

	for (const Connection& conn : connections()) {
		res += "    ";
		res += connectionToString(conn) + "\n";
	}

	return res;
}

void Net::check(const Port &super, const Port &sub, const Port &different1, const Port &different2)
{
	if (different2.second -> is_partially_open())
		throw ConstructionException(Errorcode::E_ALREADY_CONNECTED_TO_OPEN); // (2)
		//no other connections to an interface, if there is an open interface connected

	GetCorrespondingSubinterface find(*sub.second, Interface::Predicate2::ptp_nondir());
	find.go_visit(different1.second, super.second);

	assert (find.result());

	if (find.result() -> is_partially_open())
		throw ConstructionException(Errorcode::E_ALREADY_CONNECTED_TO_OPEN); // (2)
}

/* to be called with both orders */
void Net::check(const Connection &greater, const Connection &lesser)
{
	if (greater.first >= lesser.first) {
		if (greater.second >= lesser.second)
			throw ConstructionException(Errorcode::E_ILLEGAL_RECONNECTION);

		check(greater.first, lesser.first, greater.second, lesser.second);
	}
	else if (greater.second >= lesser.second) {
		check(greater.second, lesser.second, greater.first, lesser.first);
	}
}


}

