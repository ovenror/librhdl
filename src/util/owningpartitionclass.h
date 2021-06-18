/*
 * owningpartitionclass.h
 *
 *  Created on: Jun 12, 2021
 *      Author: ovenror
 */

#ifndef SRC_UTIL_OWNINGPARTITIONCLASS_H_
#define SRC_UTIL_OWNINGPARTITIONCLASS_H_

#include "util/partitionclassbase.h"

#include <memory>
#include <unordered_set>

namespace rhdl {

template <class Element>
class OwningPartitionClass : public PartitionClassBase<Element> {
public:
	using Super = PartitionClassBase<Element>;
	using Ptr = OwningPartitionClass *;
	using iterator = typename Super::iterator;
	using const_iterator = typename Super::const_iterator;
	using Owner = typename Super::Owner;

	OwningPartitionClass(Element &&element, Owner &This);
	OwningPartitionClass(Element &element, Owner &This);
	OwningPartitionClass(Element *element, Owner &This);
	OwningPartitionClass(std::unique_ptr<Element> &&element, Owner &This);
	OwningPartitionClass(OwningPartitionClass &&victim, Owner &This);
	virtual ~OwningPartitionClass() {}

	void accept(Element &&element);
	void accept(Element &element);
	void accept(Element *element);
	void accept(std::unique_ptr<Element> &&element);

	static void kill(Element &element);
};

template<class Element>
inline OwningPartitionClass<Element>::OwningPartitionClass(
		Element &&element, Owner &This)
	: Super(element, This)
{}

template<class Element>
inline OwningPartitionClass<Element>::OwningPartitionClass(
		Element& element, Owner &This)
	: Super(element, This)
{}

template<class Element>
inline OwningPartitionClass<Element>::OwningPartitionClass(
		Element* element, Owner &This)
	: Super(*element, This)
{}

template<class Element>
inline OwningPartitionClass<Element>::OwningPartitionClass(
		std::unique_ptr<Element>&& element, Owner &This)
	: Super(element.release(), This)
{
}

template<class Element>
inline OwningPartitionClass<Element>::OwningPartitionClass(
		OwningPartitionClass &&victim, Owner &This)
	: Super(std::move(victim), This)
{}

template<class Element>
inline void OwningPartitionClass<Element>::accept(Element&& element)
{
	Super::accept(element);
}

template<class Element>
inline void OwningPartitionClass<Element>::accept(Element& element)
{
	Super::accept(element);
}

template<class Element>
inline void OwningPartitionClass<Element>::accept(Element* element)
{
	Super::accept(*element);
}

template<class Element>
inline void OwningPartitionClass<Element>::accept(
		std::unique_ptr<Element>&& element)
{
	Super::accept(*element.release());
}

template<class Element>
inline void rhdl::OwningPartitionClass<Element>::kill(Element &element)
{
	delete &Super::reject(element);
}

} /* namespace rhdl */

#endif /* SRC_UTIL_OWNINGPARTITIONCLASS_H_ */
