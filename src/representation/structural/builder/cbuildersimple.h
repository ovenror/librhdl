/*
 * csimplecomplex.h
 *
 *  Created on: Aug 8, 2021
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_STRUCTURAL_BUILDER_CBUILDERSIMPLE_H_
#define SRC_REPRESENTATION_STRUCTURAL_BUILDER_CBUILDERSIMPLE_H_

#include "interface/cresult/cbase.h"

namespace rhdl {
namespace structural {
namespace builder {

class BuilderPort;
class ExistingPort;

struct CBuilderSimpleOps : public CBaseOpsBase<BuilderPort&, ExistingPort&, CBuilderSimpleOps> {
	using Super::Super;

	template <class RESULT>
	void eval_common(RESULT &result) const;

private:
	Errorcode fail_empty(std::ostream &) const;
	Errorcode fail_top(std::ostream &) const;
};

using CBuilderSimple = CBase<CBuilderSimpleOps>;

} /* namespace builder */
} /* namespace structural */
} /* namespace rhdl */

#endif /* SRC_REPRESENTATION_STRUCTURAL_BUILDER_CBUILDERSIMPLE_H_ */
