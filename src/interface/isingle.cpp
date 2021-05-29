#include "interface/isingle.h"
#include "interface/predicate.h"
#include "interface/cresult/csingle.h"
#include <iostream>
#include <typeinfo>

using namespace rhdl;

ISingle::ISingle(const std::string &name, Direction dir, bool open) :
	VisitableBase(name)

{
	auto &c = c_.content();

	c.type = RHDL_SINGLE;
	c.single.dir = static_cast<enum rhdl_direction>(dir);
	c.single.open = open;
}

// "visitor::accept()"
Interface::CResult ISingle::eq_struct_int(const Interface &other, const Predicate2 &predicate) const
{
	return other.eq_struct_int (*this, predicate.reversed());
}

Interface::CResult ISingle::eq_struct_int(const ISingle &other, const Predicate2 &predicate) const
{
	return CResult(new CSingle(*this, other, predicate));
}

bool ISingle::eq_inner_names(const Interface &other) const
{
	CResult wat(eq_struct_int (other, Predicate2::ptp_nondir()));
	bool result = wat -> success();

	return result;
}

void rhdl::ISingle::setOpen() const {
	auto &This = const_cast<ISingle &>(*this);
	This.c_.content().single.open = true;
}
