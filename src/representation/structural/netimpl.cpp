#include <interface/visitors/predicateevaluator.h>
#include <iostream>
#include "netimpl.h"
#include "entity/entity.h"
#include "temporarynet.h"
#include "interface/isingle.h"
#include "interface/predicate.h"

namespace rhdl {

NetImpl::NetImpl()
{
}

NetImpl::~NetImpl()
{
}

PartIdx NetImpl::add(const Entity *what)
{
	parts_.push_back (what);
	return parts_.size() -1;
}

bool NetImpl::connect(PartIdx from, const std::string &from_iname, PartIdx to, const std::string &to_iname)
{
	const Interface *from_interface = parts_ [from] -> interface()[from_iname];
	const Interface *to_interface = parts_ [to] -> interface()[to_iname];

	if (!from_interface)
	{
		//std::cerr << "ERROR: source interface not found" << std::endl;
		return false;
	}

	if (!to_interface)
	{
		//std::cerr << "ERROR: destination interface not found" << std::endl;
		return false;
	}

	return connect(from, from_interface, to, to_interface);
}



bool NetImpl::connect(PartIdx from, const Interface *from_interface, PartIdx to, const Interface *to_interface)
{
	if (!Entity::connectible (parts_ [from], from_interface, parts_ [to], to_interface))
	{
		return false;
	}

	Port pfrom(from, from_interface);
	Port pto(to, to_interface);
	Connection new_connection(pfrom, pto);

	for (const Connection &connection : connections_) {
		check(new_connection, connection);
		check(connection, new_connection);
	}
	
	connections_.push_back ({{from, from_interface}, {to, to_interface}});
	return true;
}

}
