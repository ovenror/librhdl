#ifndef ENTITY_H
#define ENTITY_H

#include <rhdl/construction/c/types.h>
#include "timing.h"

#include "interface/icomposite.h"

#include "simulation/simulator.h"

#include "representation/behavioral/timedbehavior.h"
#include "representation/behavioral/functionalbehavior.h"
#include "representation/netlist/netlist.h"

#include "util/util.h"
#include "util/iterable.h"
#include "util/catiterator.h"

#include "boost/iterator/transform_iterator.hpp"

#include <map>
#include <list>
#include <string>
#include <functional>

namespace rhdl {

class Blocks;
class Timing;

class Entity
{
public:
	Entity(const std::string &name, bool stateless = true);
	virtual ~Entity();

	const std::string &name() const {return name_;}
	const IComposite& interface() const;
	bool isStateless() const {return stateless_;}

	//virtual operator std::string() const =0;

	std::string fqn(const Interface *sub) const;

	/*
	 * FIXME: Does it belong here?
	 */
	const Interface* ilookup(const std::string &iname) const
	{
		return interface_[iname];
	}

	template <class RepType>
	const RepType &addRepresentation(RepType &&rep) const {
		return addRepresentation(std::make_unique<RepType>(std::move(rep)));
	}

	const Representation &addRepresentation(std::unique_ptr<Representation> &&representation) const;

	template <class RepType>
	const RepType &addRepresentation(std::unique_ptr<RepType> &&rep) const {
		std::unique_ptr<Representation> baserep = std::move(rep);
		return static_cast<const RepType &>(addRepresentation(std::move(baserep)));
	}

	const Representation *getRepresentation(
			Representation::TypeID type,
			const Representation *source = nullptr) const;

	template <class RepType>
	const RepType *getRepresentation(const Representation *source = nullptr) const {
		return dynamic_cast<const RepType *>(getRepresentation(RepType::ID, source));
	}

	const std::vector<const Representation *> getRepresentations(const Timing *timing) const;

	Representations::TypeSet representationTypes() const;

	using RepresentationContainer = std::vector<std::unique_ptr<const Representation>>;
	using TimingContainer = std::vector<std::unique_ptr<Timing>>;

	template <class Index>
	using RepresentationIndex = std::map<Index, std::vector<const Representation *>>;

	using GetRep = std::function<const Representation &(std::unique_ptr<const Representation> &)>;
	using RepresentationIterator = boost::transform_iterator<GetRep, RepresentationContainer::iterator>;

	using GetTiming = std::function<const Timing &(const std::unique_ptr<Timing> &)>;
	using TimingIterator = boost::transform_iterator<GetTiming, TimingContainer::const_iterator>;

	Iterable<RepresentationIterator> representations() const;

#if 0
	template <class... Args>
	Timing &addTiming(Args&&... args)
	{
		timings_.emplace_back(*this, std::forward<Args>(args)...);
		return timings_.back();
	}
#endif

	const Timing &addTiming() const;
	Iterable<TimingIterator> timings() const;
	const Timing *defaultTiming() const;

protected:
	friend class NewEntityHandle;

	const Representation *generate(Representation::TypeID dstType, const Representation *source = nullptr) const;
	const Representation &addRepresentation_internal(std::unique_ptr<Representation> &&representation) const;

	IComposite interface_;
	const std::string name_;
	mutable std::vector<std::unique_ptr<Timing>> timings_;
	bool stateless_;

	mutable RepresentationContainer representations_;
	mutable RepresentationIndex<RepresentationTypeID> repIdx_type_;
	mutable RepresentationIndex<const Timing *> repIdx_timing_;

private:
	RepresentationIterator getRepresentationIterator(RepresentationContainer::iterator i) const;
	TimingIterator getTimingIterator(TimingContainer::const_iterator i) const;

public:
	using C_Struct = rhdl_entity_struct;

private:
	friend class Wrapper<Entity>;
	static constexpr unsigned long C_ID = 0xE97171;
	Wrapper<Entity> c_;
};

inline const IComposite& Entity::interface() const
{
	return interface_;
}




}

#endif // ENTITY_H
