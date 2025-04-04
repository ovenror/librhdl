#include "transformations.h"
#include "transformation.h"

namespace rhdl {

Transformation::Transformation(	TypeID id, std::string name)
	: Super(name), typeID_(id),
	  srctype_("srctype", *this, c_.content().srctype),
	  dsttype_("dsttype", *this, c_.content().dsttype)
{
	c_.content().srctype = (rhdl_reptype) id.first;
	c_.content().dsttype = (rhdl_reptype) id.second;
}

Transformation::Transformation(Transformation &&moved)
	: Super(std::move(moved)), typeID_(moved.typeID_),
	  srctype_(std::move(moved.srctype_), *this, c_.content().srctype),
	  dsttype_(std::move(moved.dsttype_), *this, c_.content().dsttype)
{
	assert (srctype_ == moved.srctype_);
	assert (dsttype_ == moved.dsttype_);
}

/*
Transformation::Transformation() :
	Transformation(Transformations::INVALID, "Invalid")
{}
*/

Transformation::TypeID Transformation::typeID() const
{
	return {c_.content().srctype, c_.content().dsttype};
}

}
