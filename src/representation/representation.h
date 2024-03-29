#ifndef REPRESENTATION_H
#define REPRESENTATION_H

#include "representationtypeid.h"

#include <vector>
#include <map>
#include <memory>
#include <array>

namespace rhdl {

class Simulator;
class Timing;
class Entity;

class Representation
{
public:
	using TypeID = RepresentationTypeID;

	virtual ~Representation();

	TypeID typeID() const {return typeID_;}
	const Entity& entity() const {return entity_;}
	const Timing *timing() const {return timing_;}

	virtual bool typeCheck() const = 0;
	virtual std::unique_ptr<Simulator> makeSimulator(bool use_behavior) const;

	bool isChild(const Representation *r) const;
	bool isDescendant(const Representation *r) const;

	void breakTiming();

	std::string canonicalName() const;

protected:
	Representation(
			const Entity &entity, TypeID id, const Representation *parent,
			const Timing *timing = nullptr);

	Representation(const Entity &entity);

private:
	size_t register_descendant() const;

	const TypeID typeID_;
	const Entity &entity_;
	const Representation *parent_;
	const Timing *timing_;
	const size_t sibling_index_;
	mutable size_t num_descendants_ = 0;
};

}

#endif // REPRESENTATION_H
