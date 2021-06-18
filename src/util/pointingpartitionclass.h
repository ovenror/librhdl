/*
 * pointingpartitionclass.h
 *
 *  Created on: Jun 12, 2021
 *      Author: ovenror
 */

#ifndef SRC_UTIL_POINTINGPARTITIONCLASS_H_
#define SRC_UTIL_POINTINGPARTITIONCLASS_H_

#include "util/partitionclassbase.h"
#include <memory>

namespace rhdl {

template <class Element>
class PointingPartitionClass : public PartitionClassBase<Element> {
	using Super = PartitionClassBase<Element>;
	using Ptr = PointingPartitionClass *;
	using Owner = typename Super::Owner;

public:
	using Super::PartitionClassBase;

#if 0
	PointingPartitionClass(Element &element, Owner &This);
	PointingPartitionClass(Element *element, Owner &This);
	PointingPartitionClass(PointingPartitionClass &&victim, Owner &This);
#endif
	virtual ~PointingPartitionClass();

	void release(Super &) {delete this;}

private:
	template <class, class, class> friend class Partitionable;
};

#if 0
template<class Element>
inline PointingPartitionClass<Element>::PointingPartitionClass(
		Element& element, Owner &This)
	: Super(element, This)
{}

template<class Element>
inline PointingPartitionClass<Element>::PointingPartitionClass(
		Element* element, Owner &This)
	: Super(*element, This)
{}

template<class Element>
inline PointingPartitionClass<Element>::PointingPartitionClass(
		PointingPartitionClass &&victim, Owner &This)
	: Super(std::move(victim), This)
{}
#endif

} /* namespace rhdl */

template<class Element>
inline rhdl::PointingPartitionClass<Element>::~PointingPartitionClass()
{
	Super::clear_norelease();
}

#endif /* SRC_UTIL_POINTINGPARTITIONCLASS_H_ */
