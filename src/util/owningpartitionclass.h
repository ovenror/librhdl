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

	OwningPartitionClass(Owner &);
	OwningPartitionClass(Element &&first, Owner &);
	OwningPartitionClass(std::unique_ptr<Element> &&first, Owner &);
	OwningPartitionClass(OwningPartitionClass &&victim, Owner &);
	virtual ~OwningPartitionClass() {}

	Element &accept(Element &);
	Element &accept(Element *);

	Element &accept(Element &&);
	Element &accept(std::unique_ptr<Element> &&);

	void kill(Element &);

	void release(Super &pclass) {}

private:
	Element &acceptStray(Element &) = delete;
	Element &acceptStray(Element *) = delete;
	Element &acceptClassed(Element &) = delete;
	Element &acceptClassed(Element *) = delete;
};

template<class Element>
inline OwningPartitionClass<Element>::OwningPartitionClass(Owner &This)
	: Super(This)
{}

template<class Element>
inline OwningPartitionClass<Element>::OwningPartitionClass(
		Element &&element, Owner &This)
	: Super(new Element(element), This)
{}

template<class Element>
inline OwningPartitionClass<Element>::OwningPartitionClass(
		std::unique_ptr<Element>&& element, Owner &This)
	: Super(element.release(), This)
{}

template<class Element>
inline OwningPartitionClass<Element>::OwningPartitionClass(
		OwningPartitionClass &&victim, Owner &This)
	: Super(std::move(victim), This)
{}

template<class Element>
inline Element &OwningPartitionClass<Element>::accept(Element& element)
{
	Super::acceptClassed(element);
	return element;
}

template<class Element>
inline Element &OwningPartitionClass<Element>::accept(Element* element)
{
	Super::acceptClassed(*element);
	return *element;
}


template<class Element>
inline Element &OwningPartitionClass<Element>::accept(Element&& element)
{
	return Super::acceptStray(new Element(std::move(element)));
}

template<class Element>
inline Element &OwningPartitionClass<Element>::accept(
		std::unique_ptr<Element>&& element)
{
	return Super::acceptStray(element.release());
}

template<class Element>
inline void rhdl::OwningPartitionClass<Element>::kill(Element &element)
{
	delete &Super::reject(element);
}

} /* namespace rhdl */

#endif /* SRC_UTIL_OWNINGPARTITIONCLASS_H_ */
