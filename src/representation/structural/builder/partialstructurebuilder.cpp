#include "partialstructurebuilder.h"
#include "structure.h"

#include "entity/entity.h"

#include <algorithm>
#include <iostream>

namespace rhdl::structural::builder {

class Port;
class Part;

PartialStructureBuilder::~PartialStructureBuilder() {}

void PartialStructureBuilder::combineWith(StructureBuilder &b)
{
	b.ingest(*this);
}

void PartialStructureBuilder::combineWith(CompleteStructureBuilder &s)
{
	s.ingest(*this);
}

}
