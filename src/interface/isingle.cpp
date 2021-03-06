#include "isingle.h"
#include "predicate.h"
#include "cresult/csingle.h"

#include <cassert>
#include <iostream>
#include <typeinfo>

using namespace rhdl;

ISingle::ISingle(const std::string &name, Direction dir) :
	VisitableBase(name)

{
	auto &c = c_.content();

	c.type = RHDL_SINGLE;
	c.single.dir = static_cast<enum rhdl_direction>(dir);
}

bool ISingle::eq_inner_names(const Interface &other) const
{
	CResult wat(compatTo(other, Predicate2::ptp(false)));
	bool result = wat -> success();

	return result;
}
