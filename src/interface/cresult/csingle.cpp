#include "csingle.h"
#include "cdirection.h"
#include "interface/isingle.h"

namespace rhdl {

CSingle::~CSingle()
{
}

bool CSingleOps::check_open() const
{
	return predicate.samedir_ || !lhs.is_open() || !rhs.is_open();
}

void CSingle::eval_int() const
{
	use(CDirection(ops_.lhs.compositeDirection(), ops_.rhs.compositeDirection(), ops_.predicate));

	if (ops_.check_open())
		return;

	if (!success()) {
		incompatibility(Errorcode::E_INTERFACES_NOT_COMPATIBLE);
		return;
	}

	incompatibility(Errorcode::E_OPEN_TO_OPEN) << "* both are open, which is not allowed with opposite directions";
}

bool CSingleOps::compatible() const
{
	return CDirection::compatible(
				lhs.compositeDirection(), rhs.compositeDirection(), predicate
			)
		&& check_open();
}

}
