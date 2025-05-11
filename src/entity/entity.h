#ifndef ENTITY_H
#define ENTITY_H

#include <rhdl/construction/c/types.h>

#include "representationcontainer.h"
#include "timing.h"

#include "interface/icomposite.h"

#include "simulation/simulator.h"

#include "representation/behavioral/timedbehavior.h"
#include "representation/behavioral/functionalbehavior.h"
#include "representation/netlist/netlist.h"
#include "util/util.h"
#include "util/iterable.h"
#include "util/catiterator.h"
#include "c_api/typedcomplexcobject.h"

#include "boost/iterator/transform_iterator.hpp"

#include <map>
#include <list>
#include <string>
#include <functional>

namespace rhdl {

class Timing;

class Entity : public TypedComplexCObject<Entity, rhdl_entity_struct, false>
{
public:
	Entity(
			const std::string &name,
			std::vector<const Interface *>, bool stateless = true);

	virtual ~Entity();

	virtual Entity &cast() override {return *this;}

	const IComposite& interface() const;
	bool isStateless() const {return stateless_;}

	//virtual operator std::string() const =0;

	std::string fqn(const Interface *sub) const;

	template <class RepType>
	const RepType &addRepresentation(RepType &&rep) const {
		return addRepresentation(std::make_unique<RepType>(std::move(rep)));
	}

	const Representation &addRepresentation(std::unique_ptr<Representation> &&representation) const;

	template <class RepType>
	const RepType &addRepresentation(std::unique_ptr<RepType> &&rep) const {
		const RepType &derivedrep = *rep;
		std::unique_ptr<Representation> baserep = std::move(rep);
		assert(baserep.get() != nullptr);
		addRepresentation(std::move(baserep));
		return derivedrep;
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
	const Representation *generate(Representation::TypeID dstType, const Representation *source = nullptr) const;
	const Representation &addRepresentation_internal(std::unique_ptr<Representation> &&representation) const;

	const IComposite interface_;
	mutable std::vector<std::unique_ptr<Timing>> timings_;
	bool stateless_;

	mutable RepresentationContainer representations_;
	mutable RepresentationIndex<RepresentationTypeID> repIdx_type_;
	mutable RepresentationIndex<const Timing *> repIdx_timing_;

private:
	virtual operator Entity &() override {return *this;}
	virtual operator const Entity &() const override {return *this;}

	RepresentationIterator getRepresentationIterator(RepresentationContainer::iterator i) const;
	TimingIterator getTimingIterator(TimingContainer::const_iterator i) const;
};

inline const IComposite& Entity::interface() const
{
	return interface_;
}

}

#endif // ENTITY_H
