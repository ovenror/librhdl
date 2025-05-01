#include "isingle.h"
#include "predicate.h"
#include "cresult/csingle.h"
#include "c_api/namespace.h"
#include "c_api/typedcvalue.h"

#include <cassert>
#include <iostream>
#include <typeinfo>

using namespace rhdl;

ISingle::ISingle(const std::string &name, Direction dir) :
	VisitableBase(name), dir_("direction", *this, c_ptr() -> single.dir)
{
	setDictionary(dictionary::DereferencingDictionaryAdapter<decltype(dict_)>(dict_));

	auto &c = *c_ptr();

	c.type = RHDL_SINGLE;
	c.single.dir = static_cast<enum rhdl_direction>(dir);
}

rhdl::ISingle::~ISingle() {}

bool ISingle::eq_inner_names(const Interface &other) const
{
	CResult wat(compatTo(other, Predicate2::ptp(false)));
	bool result = wat -> success();

	return result;
}

const CObject& rhdl::ISingle::add(const CValue &v)
{
	return *Super::add(dict_, &v);
}

const CObject& rhdl::ISingle::add_after_move(const CValue &v)
{
	assert(0);
	return *Super::replace(dict_, &v);
}
