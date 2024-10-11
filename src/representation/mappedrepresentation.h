/*
 * poststructuralrepresentation.h
 *
 *  Created on: Jun 7, 2022
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_MAPPEDREPRESENTATION_H_
#define SRC_REPRESENTATION_MAPPEDREPRESENTATION_H_

#include "representationbase.h"

#include <algorithm>
#include <map>

namespace rhdl {

class ISingle;

template <class RepType, class ELEMENT_REF>
class MappedRepresentation : public RepresentationBase<RepType> {
	using Super = RepresentationBase<RepType>;

public:
	using ElementRef = 	ELEMENT_REF;
	using InterfaceMap = std::map<const ISingle *, ElementRef>;

	MappedRepresentation(
			const Entity &, const Representation *parent, const Timing *,
			InterfaceMap);
	MappedRepresentation(MappedRepresentation &&) = default;

	using Super::Super;
	virtual ~MappedRepresentation() {}

	const InterfaceMap &ifaceMap() const {return ifaceMap_;}

	static void remap(InterfaceMap &, std::map<ElementRef, ElementRef>);

protected:
	virtual bool existsElementRef(ELEMENT_REF) = 0;
	void checkIfaceMap();

	InterfaceMap ifaceMap_;
};

template<class RepType, class ELEMENT_REF>
inline MappedRepresentation<RepType, ELEMENT_REF>::MappedRepresentation(
		const Entity &entity, const Representation *parent,
		const Timing *timing, InterfaceMap ifaceMap)
	: RepresentationBase<RepType>(entity, parent, timing),
	  ifaceMap_(std::move(ifaceMap))
{}

template<class RepType, class ELEMENT_REF>
inline void rhdl::MappedRepresentation<RepType, ELEMENT_REF>::remap(
		InterfaceMap &mapping, std::map<ElementRef, ElementRef> changes)
{
	for (auto &kv : mapping) {
		kv.second = changes.at(kv.second);
	}
}

template<class RepType, class ELEMENT_REF>
inline void MappedRepresentation<RepType, ELEMENT_REF>::checkIfaceMap()
{
	assert (std::all_of(ifaceMap_.begin(), ifaceMap_.end(), [&](auto &kv){
		return existsElementRef(kv.second);
	}));
}

} /* namespace rhdl */


#endif /* SRC_REPRESENTATION_MAPPEDREPRESENTATION_H_ */
