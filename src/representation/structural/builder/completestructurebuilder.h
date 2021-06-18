/*
 * completestructurebuilder.h
 *
 *  Created on: Aug 11, 2021
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_STRUCTURAL_BUILDER_COMPLETESTRUCTUREBUILDER_H_
#define SRC_REPRESENTATION_STRUCTURAL_BUILDER_COMPLETESTRUCTUREBUILDER_H_

#include "structurebuilder.h"
#include "../structure.h"

namespace rhdl {
namespace structural {
namespace builder {

class CompleteStructureBuilder : public StructureBuilder {
public:
	CompleteStructureBuilder(bool stateless);
	virtual ~CompleteStructureBuilder();

	void combineWith(StructureBuilder &b) override;
	void combineWith(CompleteStructureBuilder &s) override;

	structural::Structure construct(Structure &, const Entity &) const;

private:
	void setStateful() override;
};

} /* namespace builder */
} /* namespace structural */
} /* namespace rhdl */

#endif /* SRC_REPRESENTATION_STRUCTURAL_BUILDER_COMPLETESTRUCTUREBUILDER_H_ */
