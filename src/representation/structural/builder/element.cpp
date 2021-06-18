/*
 * index.cpp
 *
 *  Created on: Jun 13, 2021
 *      Author: ovenror
 */

#include "complexport.h"
#include "portscreator.h"
#include "element.h"
#include "structurebuilder.h"

#include "interface/icomposite.h"
#include "entity/entity.h"

#include <cassert>

namespace rhdl::structural::builder {

Element::~Element() {}

void Element::invalidate()
{
	topPort().invalidateHandles();
}

std::ostream& operator <<(std::ostream &os,
		const Element &element)
{
	element.toStream(os);
	return os;
}

ConnectionPredicate Element::predicate(
		const Element &from, const Element &to, bool directional)
{
	bool sfrom = from.isTheStructure();
	bool sto = to.isTheStructure();

	assert (!sfrom || !sto);

	if (sfrom)
		return ConnectionPredicate::etp(directional);

	if (sto)
		return ConnectionPredicate::pte(directional);

	return ConnectionPredicate::ptp(directional);
}

Port& Element::topPort()
{
	return top().port();
}


void Element::combineBuilders(Element &lhs, Element &rhs)
{
	StructureBuilder::combineIffNotSame(lhs.builder(), rhs.builder());
}

} /* namespace rhdl::structural::builder */
