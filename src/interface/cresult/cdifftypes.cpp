#include "cdifftypes.h"

namespace rhdl {

CDiffTypes::~CDiffTypes() {}

void CDiffTypes::eval_int() const
{
	incompatibility(Errorcode::E_DIFFERENT_TYPES)
			<< "* different types (" << typeid(ops_.lhs).name()
			<< " and " << typeid(ops_.rhs).name() << std::endl;
}

}
