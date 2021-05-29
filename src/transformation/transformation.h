#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include "transformationtypeid.h"
#include "util/list.h"
#include <memory>

namespace rhdl {

class Representation;
class Blocks;
class Netlist;
class Structural;
class TimedBehavior;
class FunctionalBehavior;

class StructureToNetlist;
class BGTree;

class Transformation
{
public:
	using TypeID = TransformationTypeID;

	Transformation(TypeID id);
	Transformation();

	TypeID typeID() const {return typeID_;}

	virtual bool typeCheck() const = 0;

	virtual std::unique_ptr<Representation> execute(const Representation &source) const = 0;

private:
	TypeID typeID_;
};

}

#endif // TRANSFORMATION_H
