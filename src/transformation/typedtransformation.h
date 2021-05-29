#ifndef SYNTHESIZER_H
#define SYNTHESIZER_H

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

	TypedTransformation() : Transformation(ID) {}

	virtual bool typeCheck() const override {return typeID() == ID;}

	virtual std::unique_ptr<Representation> execute(const Representation &source) const override {
		auto *typed_source = dynamic_cast<const FromRep *>(&source);
		assert (typed_source);
		assert (typed_source -> typeID() == FromRep::ID);
		return std::make_unique<ToRep>(execute(*typed_source));
	}

protected:
	virtual ToRep execute(const FromRep &source) const = 0;
};

}

#endif // SYNTHESIZER_H
