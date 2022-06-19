#include "csingle.h"
#include "cdirection.h"
#include "interface/isingle.h"

namespace rhdl {

CSingle::~CSingle()
{
}

void CSingle::eval_int() const
{
	use(CDirection(ops_.lhs.compositeDirection(), ops_.rhs.compositeDirection(), ops_.predicate));
}

bool CSingleOps::compatible() const
{
	return CDirection::compatible(
				lhs.compositeDirection(), rhs.compositeDirection(), predicate
			);
}

}
