#include "cdifftypes.h"

namespace rhdl {

CDiffTypes::CDiffTypes(const Interface &from, const Interface &to, const Predicate &predicate) :
	BASE(from, to, predicate)
{
}

CDiffTypes::~CDiffTypes()
{

}

void CDiffTypes::eval_int() const
{
	success_ = false;
	ec_ = Errorcode::E_DIFFERENT_TYPES;
	msg_ << "* different types (" << typeid(from_).name() << " and " << typeid(to_).name() << std::endl;
}

}
