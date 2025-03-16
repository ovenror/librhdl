#include "transformations.h"
#include "transformation.h"

namespace rhdl {

Transformation::Transformation(TypeID id, std::string name) :
	ValueObject(name), typeID_(id),
	srctype_("srctype", c_.content().srctype),
	dsttype_("dsttype", c_.content().dsttype)
{
	c_.content().srctype = (rhdl_reptype) id.first;
	c_.content().dsttype = (rhdl_reptype) id.second;
	add(&srctype_);
	add(&dsttype_);
}

Transformation::Transformation() :
	Transformation(Transformations::INVALID, "Invalid")
{}

Transformation::TypeID Transformation::typeID() const
{
	return {c_.content().srctype, c_.content().dsttype};
}

}
