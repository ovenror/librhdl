#include "transformations.h"
#include "transformation.h"

namespace rhdl {

Transformation::Transformation(TypeID id) :
	typeID_(id)
{}

Transformation::Transformation() :
	Transformation(Transformations::INVALID)
{}

}
