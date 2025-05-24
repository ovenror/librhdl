#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include <c_api/referencedcvalue.h>
#include "transformationtypeid.h"

#include "util/list.h"

#include "c_api/typedvalueobject.h"
#include <memory>

namespace rhdl {

class Namespace;
class Representation;

class Transformation : public TypedValueObject<Transformation, rhdl_transformation>
{
	using Super = TypedValueObject<Transformation, rhdl_transformation>;

public:
	using TypeID = TransformationTypeID;

	Transformation(TypeID id, std::string name);
	Transformation(Transformation &&);
	//Transformation();

	TypeID typeID() const;

	virtual bool typeCheck() const = 0;

	virtual std::unique_ptr<Representation> execute(
			const Representation &source, const std::string &result_name = "") const = 0;

private:
	TypeID typeID_;
	ReferencedCValue<rhdl_reptype> srctype_;
	ReferencedCValue<rhdl_reptype> dsttype_;

};

}

#endif // TRANSFORMATION_H
