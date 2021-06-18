/*
 * cpeerdirection.cpp
 *
 *  Created on: Nov 16, 2021
 *      Author: ovenror
 */

#include "cpeerdirection.h"
#include "port.h"

namespace rhdl::structural::builder {

CPeerDirection::CPeerDirection(
		const Port &peer, const ConnectionPredicate &predicate)
	: COpDirection(peer.direction(), predicate.reversed())
{
	// because this should only be called from a new structure element
	assert (predicate.samedir_);
}

CPeerDirection::~CPeerDirection() {}

bool CPeerDirection::compatible(
		const Port &peer, const ConnectionPredicate &predicate)
{
	return COpDirection::compatible(peer.direction(), predicate.reversed());
}

} /* namespace rhdl */
