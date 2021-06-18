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
	PointingPartitionClass(Element &element, Owner &This);
	PointingPartitionClass(Element *element, Owner &This);
	PointingPartitionClass(PointingPartitionClass &&victim, Owner &This);
	virtual ~PointingPartitionClass() {}

	void accept(Element &element);
	void accept(Element *element);

private:
	template <class, class, class> friend class Partitionable;
};

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

template<class Element>
inline void PointingPartitionClass<Element>::accept(Element& element)
{
	if (element.pcOwner_)
		Super::accept(element);
	else
		Super::acceptStray(element);
}

template<class Element>
inline void PointingPartitionClass<Element>::accept(Element* element)
{
	Super::accept(*element);
}

} /* namespace rhdl */

#endif /* SRC_UTIL_POINTINGPARTITIONCLASS_H_ */
