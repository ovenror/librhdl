/*
 * csimplecomplex.cpp
 *
 *  Created on: Aug 8, 2021
 *      Author: ovenror
 */

#include "cbuildersimple.h"
#include "cpeerdirection.h"

#include "builderport.h"
#include "existingport.h"

#include "interface/compositedirection.h"

namespace rhdl {
namespace structural {
namespace builder {

template<class RESULT>
void CBuilderSimpleOps::eval_common(
		RESULT &result) const
{
	if (!lhs.enclosing()) {
		issue(&This::fail_top, result);
		return;
	}

	if (!lhs.enclosed().empty()) {
		issue(&This::fail_empty, result);
		return;
	}

	/* not necessary for this method's correctness */
	assert (lhs.direction().free());

	use<CPeerDirection>(result, rhs, predicate);
}

template void CBuilderSimpleOps::eval_common(bool &) const;
template void CBuilderSimpleOps::eval_common(const CBuilderSimple &) const;


Errorcode CBuilderSimpleOps::fail_empty(std::ostream &os) const
{
	os << lhs
	   << " already has subcomponents and is therefore incompatible to "
	   << rhs;
	return Errorcode::E_DIFFERENT_TYPES;
}

Errorcode CBuilderSimpleOps::fail_top(std::ostream &os) const
{
	os << lhs
	   << " is the top builder port, which has to be composite, whereas "
	   << rhs << " is a single port";
	return Errorcode::E_DIFFERENT_TYPES;
}

} /* namespace builder */
} /* namespace structural */
} /* namespace rhdl */
