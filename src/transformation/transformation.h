#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include "transformationtypeid.h"
#include "util/list.h"
#include "c_api/valueobject.h"
#include "c_api/typedcvalue.h"
#include <memory>

namespace rhdl {

class Representation;

class Transformation : public ValueObject<rhdl_transformation, RHDL_TRANSFORMATION>
{
public:
	using TypeID = TransformationTypeID;

	Transformation(TypeID id, std::string name);
	Transformation();

	TypeID typeID() const;

	virtual bool typeCheck() const = 0;

	virtual std::unique_ptr<Representation> execute(const Representation &source) const = 0;

private:
	TypeID typeID_;
	TypedCValue<rhdl_reptype> srctype_;
	TypedCValue<rhdl_reptype> dsttype_;

};

}

#endif // TRANSFORMATION_H
