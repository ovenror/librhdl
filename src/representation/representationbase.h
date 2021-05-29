#ifndef REPRESENTATIONBASE_H
#define REPRESENTATIONBASE_H

#include "representation.h"
#include "representations.h"
#include "transformation/transformer.h"
#include "transformation/typedtransformation.h"

namespace rhdl {

class Entity;

template <class RepType>
struct RepresentationBase : public Representation {
	static const TypeID ID{Representations::TypeToID<RepType>::value};

	RepresentationBase(
			const Entity &entity, const Representation *parent,
			const Timing *timing)
		:
		  Representation(entity, ID, parent, timing)
	{
		static_assert(RepType::ID == ID, "CRTP!");
	}

	virtual bool typeCheck() const override {return typeID() == ID;}
};

}

#endif // REPRESENTATIONBASE_H
