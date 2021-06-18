/*
 * cfirstdirection.h
 *
 *  Created on: Nov 13, 2021
 *      Author: ovenror
 */

#ifndef SRC_INTERFACE_CRESULT_COPDIRECTION_H_
#define SRC_INTERFACE_CRESULT_COPDIRECTION_H_

#include "compatibilityresult.h"
#include "interface/compositedirection.h"
#include "construction/connectionpredicate.h"

namespace rhdl {

class CompositeDirection;
class ConnectionPredicate;

class COpDirection: public CompatibilityResult {
public:
	COpDirection(const CompositeDirection &dir, const ConnectionPredicate &predicate);
	virtual ~COpDirection();

	virtual void eval_int() const override;
	static bool compatible(const CompositeDirection &, const ConnectionPredicate &);

private:
	static bool check_op(const CompositeDirection &, const ConnectionPredicate &);

	const CompositeDirection dir_;
	const ConnectionPredicate predicate_;
};

} /* namespace rhdl */

#endif /* SRC_INTERFACE_CRESULT_COPDIRECTION_H_ */
