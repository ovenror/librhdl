/*
 * cfirstdirection.cpp
 *
 *  Created on: Nov 13, 2021
 *      Author: ovenror
 */

#include "copdirection.h"
#include "../compositedirection.h"
#include "construction/connectionpredicate.h"

namespace rhdl {

COpDirection::COpDirection(const CompositeDirection &dir, const ConnectionPredicate &p)
	: dir_(dir), predicate_(p)
{}

COpDirection::~COpDirection() {
}

void COpDirection::eval_int() const
{
	if (!check_op(dir_, predicate_))
		incompatibility(Errorcode::E_DIRECTION_OPPOSES_OPERATOR) << "Direction opposes operator.";
}

bool COpDirection::compatible(
		const CompositeDirection &dir, const ConnectionPredicate &p)
{
	return check_op(dir, p);
}

bool COpDirection::check_op(
		const CompositeDirection &dir, const ConnectionPredicate &p)
{
	return !p.check_first_dir_ ||
			(p.first_dir_== SingleDirection::OUT ? !dir.in() : !dir.out());
}

} /* namespace rhdl */
