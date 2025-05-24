#ifndef SYNTHESIZER_H
#define SYNTHESIZER_H

#include <c_api/typedvalueobject.h>
#include "representation/representationtypeid.h"
#include "transformation.h"

#include <memory>
#include <cassert>

namespace rhdl {

class Representation;

template <class RepType>
class RepresentationBase;

template <class FromRep, class ToRep>
class TypedTransformation : public Transformation
{
public:
	inline static constexpr TypeID ID{FromRep::ID, ToRep::ID};

	TypedTransformation(std::string name) : Transformation(ID, name) {}

	virtual bool typeCheck() const override {return typeID() == ID;}

	virtual std::unique_ptr<Representation> execute(
			const Representation &source,
			const std::string &result_name = "") const override
	{
		auto *typed_source = dynamic_cast<const FromRep *>(&source);
		assert (typed_source);
		assert (typed_source -> typeID() == FromRep::ID);
		return execute(*typed_source, std::move(result_name));
	}

protected:
	virtual std::unique_ptr<ToRep> execute(
			const FromRep &source, const std::string &result_name = "") const = 0;
};

}

#endif // SYNTHESIZER_H
