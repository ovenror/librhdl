#ifndef TEMPORARYNET_H
#define TEMPORARYNET_H

#include "structurebuilder.h"

namespace rhdl {

namespace structural::builder {

class PartialStructureBuilder : public StructureBuilder
{
public:
	virtual ~PartialStructureBuilder();

	virtual void combineWith(StructureBuilder &b) override;
	virtual void combineWith(CompleteStructureBuilder &s) override;
};

}}

#endif // TEMPORARYNET_H
