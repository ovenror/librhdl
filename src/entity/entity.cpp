#include <entity/representationcontainer.h>
#include "entity.h"
#include "timing.h"

#include <rhdl/construction/constructionexception.h>

#include "c_api/namespace.h"

#include "interface/predicate.h"

#include "transformation/transformer.h"

#include "representation/representations.h"

#include <iostream>

using namespace rhdl;

Entity::Entity(
		const std::string &name, std::vector<const Interface *> components,
		bool stateless
		)
		: CObjectImpl(name), interface_("interface", components),
		  stateless_(stateless)
{
	repIdx_timing_[nullptr];
	add(&interface_);
	add(&representations_);
	c_.content().iface = c_ptr(static_cast<const Interface &>(interface_));
}

Entity::~Entity() {}

const Representation &Entity::addRepresentation(std::unique_ptr<Representation> &&representation) const
{
	using blocks::Blocks;
	using netlist::Netlist;
	using structural::Structure;

	const Representation &result = addRepresentation_internal(std::move(representation));

	if (!isStateless())
		return result;

	auto type = result.typeID();

	const Netlist *netlist = nullptr;

	switch (type) {
	case Representations::TypeToID<Structure>::value:
	case Representations::TypeToID<Blocks>::value:
		netlist = getRepresentation<Netlist>(&result);
		break;
	case Netlist::ID:
		netlist = static_cast<const Netlist *>(&result);
		break;
	default:
		return result;
	}

	assert(netlist); //we do not have block -> netlist transformation (yet)

	if (netlist -> hasCycles())
		throw ConstructionException(Errorcode::E_NETLIST_CONTAINS_CYCLES);

	return result;
}

const Representation &Entity::addRepresentation_internal(std::unique_ptr<Representation> &&representation) const
{
	assert (representation -> typeCheck());
	assert (repIdx_timing_.find(representation -> timing()) != repIdx_timing_.end());

	representations_.push_back(std::move(representation));
	const Representation &result = *representations_.back();

	repIdx_type_[result.typeID()].push_back(&result);
	repIdx_timing_[result.timing()].push_back(&result);

	return result;
}

std::string Entity::fqn(const Interface *sub) const
{
	return sub -> qualifiedName(interface_);
}

const Representation *Entity::getRepresentation(Representation::TypeID type, const Representation *source) const
{
	assert (!source || (&source -> entity() == this && source -> typeID() != type));

	if (repIdx_type_.find(type) != repIdx_type_.end()) {
		const auto &list = repIdx_type_.at(type);

		assert (!list.empty());

		if (!source)
			return list[0];

		for (const Representation *candidate : list) {
			if (candidate -> isDescendant(source))
				return candidate;
		}
	}

	const Representation *result = generate(type, source);
	return result;
}

const std::vector<const Representation *> Entity::getRepresentations(const Timing *timing) const
{
	assert (&timing -> entity() == this);
	assert (repIdx_timing_.find(timing) != repIdx_timing_.end());

	return repIdx_timing_.at(timing);
}

Representations::TypeSet Entity::representationTypes() const
{
	Representations::TypeSet result;

	for (const auto &kv : repIdx_type_) {
		assert (!kv.second.empty());
		result.set(kv.first);
	}

	return result;
}


Entity::RepresentationIterator Entity::getRepresentationIterator(RepresentationContainer::iterator i) const
{
	using function = const Representation &(std::unique_ptr<const Representation> &);
	using src = RepresentationContainer::iterator;
	return boost::make_transform_iterator(i, [](std::unique_ptr<const Representation> &pr) -> const Representation & {return *pr;});
}

Entity::TimingIterator Entity::getTimingIterator(TimingContainer::const_iterator i) const
{
	return boost::make_transform_iterator(i, [](const std::unique_ptr<Timing> &pt) -> const Timing & {return *pt;});
}

Iterable<Entity::RepresentationIterator> Entity::representations() const
{
	RepresentationIterator begin = getRepresentationIterator(representations_.begin());
	RepresentationIterator end = getRepresentationIterator(representations_.end());

	return Iterable<RepresentationIterator>(begin, end);
}

const Timing &Entity::addTiming() const
{
	timings_.push_back(std::make_unique<Timing>(*this));
	const Timing &result = *timings_.back();
	repIdx_timing_[&result]; //touch
	return result;
}

Iterable<Entity::TimingIterator> Entity::timings() const
{
	TimingIterator begin = getTimingIterator(timings_.cbegin());
	TimingIterator end = getTimingIterator(timings_.cend());

	return Iterable<TimingIterator>(begin, end);
}

const Timing *Entity::defaultTiming() const
{
	if (timings_.empty())
		return nullptr;

	return timings_.begin() -> get();
}

const Representation *Entity::generate(Representation::TypeID dstType, const Representation *source) const
{
	const Transformer::PathResult *pathResult = nullptr;

	if (source)
		pathResult = transformer.getTransformationPath(dstType, source -> typeID());
	else
		pathResult = transformer.getTransformationPath(dstType, representationTypes());

	if (!pathResult)
		return nullptr;

	const TransformationTypeID &endpoints = pathResult -> first;
	const Transformer::Path &path = pathResult -> second;

	RepresentationTypeID existingType = endpoints.first;
	const Representation *result = getRepresentation(existingType);

	for (Representation::TypeID vertexType : path) {
		assert (result);
		result = &addRepresentation_internal(transformer.transform(*result, {existingType, vertexType}));
		existingType = vertexType;
	}

	assert (result);
	assert (result -> typeID() == dstType);
	return result;
}
