#ifndef REPRESENTATION_H
#define REPRESENTATION_H

#include <rhdl/construction/c/types.h>

#include "representationtypeid.h"

#include "c_api/typedvalueobject.h"
#include "c_api/cacheddynamiccvalue.h"
#include "c_api/referencedcvalue.h"
#include "c_api/storedcvalue.h"
#include "c_api/cobjectreference.h"

#include <vector>
#include <map>
#include <memory>
#include <array>

namespace rhdl {

class Simulator;
class Timing;
class Entity;

class Representation
		: public TypedValueObject<Representation, rhdl_representation>
{
	using Super = TypedValueObject<Representation, rhdl_representation>;

public:
	using TypeID = RepresentationTypeID;

	Representation(Representation &&);
	virtual ~Representation();

	Representation &cast() override {return *this;}

	TypeID typeID() const {return typeID_;}
	const Entity& entity() const {return entity_;}
	const Timing *timing() const {return timing_;}

	virtual bool typeCheck() const = 0;
	virtual std::unique_ptr<Simulator> makeSimulator(bool use_behavior) const;

	bool isChild(const Representation *r) const;
	bool isDescendant(const Representation *r) const;

	void breakTiming();

	std::string canonicalName(
			const Entity &entity, TypeID type,
			const Representation *parent) const;

	operator const std::string&() const {return content_;}

protected:
	Representation(
			const Entity &entity, TypeID id, const Representation *parent,
			const Timing *timing = nullptr, const std::string &name = "");

	Representation(const Entity &entity);

	virtual void compute_content(std::string&) const;
	const Representation *parent() const {return parent_;}

private:
	size_t register_descendant() const;
	const rhdl_representation *&cparent() {return c_.content().parent;}

	const TypeID typeID_;
	const Entity &entity_;
	const Timing *timing_;
	const size_t sibling_index_;
	mutable size_t num_descendants_ = 0;
	ReferencedCValue<rhdl_reptype> reptype_;
	CObjectReference<StoredCValue, const Representation *> parent_;
	CachedDynamicCValue<Representation, std::string> content_;
};

}

#endif // REPRESENTATION_H
