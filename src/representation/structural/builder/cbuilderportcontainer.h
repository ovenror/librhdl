/*
 * cbuilder.h
 *
 *  Created on: Jul 26, 2021
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_STRUCTURAL_BUILDER_CBUILDERPORTCONTAINER_H_
#define SRC_REPRESENTATION_STRUCTURAL_BUILDER_CBUILDERPORTCONTAINER_H_

#include "interface/cresult/cbase.h"
#include "construction/connectionpredicate.h"

#include <array>
#include <vector>

namespace rhdl {
namespace structural {
namespace builder {

class BuilderPort;
class PortContainer;
class Port;

class CBuilderPortContainerOps : public CBaseOpsBase<
	BuilderPort&, PortContainer&, CBuilderPortContainerOps>
{
	using Mapping = std::vector<std::array<const Port *, 2>>;
	std::pair<Mapping, CompatibilityResult> mapTo(PortContainer &peer) const;

public:
	using Super::Super;

	const Mapping &mapping() const {return mapping_;}

	template <class RESULT>
	void eval_common(RESULT &) const;

private:
	Errorcode fail_num(std::ostream &) const;
	Errorcode found_none(std::ostream &, const Port &sub) const;
	Errorcode found_multiple(std::ostream &, const Port &sub) const;

	mutable Mapping mapping_;
};

using CBuilderPortContainer = CBase<CBuilderPortContainerOps>;

} /* namespace builder */
} /* namespace structural */
} /* namespace rhdl */

#endif /* SRC_REPRESENTATION_STRUCTURAL_BUILDER_CBUILDERPORTCONTAINER_H_ */
