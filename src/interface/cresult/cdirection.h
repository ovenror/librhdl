/*
 * cdirection.h
 *
 *  Created on: Sep 12, 2021
 *      Author: ovenror
 */

#ifndef SRC_INTERFACE_CRESULT_CDIRECTION_H_
#define SRC_INTERFACE_CRESULT_CDIRECTION_H_

#include "cbase.h"

namespace rhdl {

class CompositeDirection;

struct CDirectionOps : public CSametypeOpsBase<CompositeDirection, CDirectionOps> {
	using Super::Super;

	template <class RESULT>
	void eval_common(RESULT &result) const;

private:
	bool check_fit() const;
	bool check_op() const;

	Errorcode fail_fit(std::ostream &) const;
	Errorcode fail_op(std::ostream &) const;
};

using CDirection = CBase<CDirectionOps>;

} /* namespace rhdl */

#endif /* SRC_INTERFACE_CRESULT_CDIRECTION_H_ */
