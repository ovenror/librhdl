#ifndef REPRESENTATION_H
#define REPRESENTATION_H

#include <c_api/valueobject.h>
#include <rhdl/construction/c/types.h>

#include "representationtypeid.h"
#include "c_api/typedcvalue.h"
#include "c_api/valueobject.h"

#include <vector>
#include <map>
#include <memory>
#include <array>

namespace rhdl {

class Simulator;
class Timing;
class Entity;

class Representation
		: public ValueObject<rhdl_representation, RHDL_REPRESENTATION>
{
	using Super = ValueObject<rhdl_representation, RHDL_REPRESENTATION>;

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

protected:
	Representation(
			const Entity &entity, TypeID id, const Representation *parent,
			const Timing *timing = nullptr);

	Representation(const Entity &entity);

private:
	size_t register_descendant() const;
	void compute_content(std::string&) const;

	const TypeID typeID_;
	const Entity &entity_;
	const Representation *parent_;
	const Timing *timing_;
	const size_t sibling_index_;
	mutable size_t num_descendants_ = 0;
	TypedCValue<rhdl_reptype> reptype_;
	Cached<std::string, Representation> content_cache_;
	TypedCValue<const char *> content_;
};

}

#endif // REPRESENTATION_H
