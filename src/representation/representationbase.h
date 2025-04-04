#ifndef REPRESENTATIONBASE_H
#define REPRESENTATIONBASE_H

#include <c_api/typedvalueobject.h>
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
	RepresentationBase(RepresentationBase &&moved) = default;

	virtual bool typeCheck() const override {return typeID() == ID;}

	template <class SourceRep>
	static std::unique_ptr<RepType> make(const SourceRep &);
};

template <class RepType>
template <class SourceRep>
inline std::unique_ptr<RepType> rhdl::RepresentationBase<RepType>::make(
		const SourceRep &source)
{
	return std::make_unique<RepType>(source);
}

}

#endif // REPRESENTATIONBASE_H
