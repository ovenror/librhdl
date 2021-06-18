/*
 * cdirection.cpp
 *
 *  Created on: Sep 12, 2021
 *      Author: ovenror
 */

#include "cdirection.h"
#include "copdirection.h"
#include "../compositedirection.h"

namespace rhdl {

bool CDirectionOps::check_fit() const
{
	return predicate.samedir_ ? lhs == rhs : lhs == rhs.complement();
}

bool CDirectionOps::check_op() const
{
	return !predicate.check_first_dir_ ||
			predicate.first_dir_== SingleDirection::OUT ? lhs.out() : lhs.in();
}

template <class RESULT>
void CDirectionOps::eval_common(RESULT &result) const {
	if (!check_fit()) {
		issue(&This::fail_fit, result);
		return;
	}

	use<COpDirection>(result, lhs, predicate);
}

template void CDirectionOps::eval_common(bool &) const;
template void CDirectionOps::eval_common(const CDirection &) const;

Errorcode CDirectionOps::fail_fit(std::ostream &os) const
{
	if (predicate.samedir_)
		os << "* opposite directions, should be the same";
	else
		os << "* same directions, should be opposite";

	return Errorcode::E_INCOMPATIBLE_DIRECTIONS;
}

Errorcode CDirectionOps::fail_op(std::ostream &os) const
{
	os << "* direction of first should be " << predicate.first_dir_
	   << " but is " << lhs;
	return Errorcode::E_DIRECTION_OPPOSES_OPERATOR;
}

} /* namespace rhdl */
