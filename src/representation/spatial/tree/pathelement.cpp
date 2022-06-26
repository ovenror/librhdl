/*
 * pathelement.cpp
 *
 *  Created on: Jun 23, 2022
 *      Author: ovenror
 */

#include "pathelement.h"
#include "segment.h"

namespace rhdl::spatial {

PathElement::PathElement(const Current &current, blocks::index_t position)
	: PathElement(*current.first, current.second, position)
{}

PathElement::PathElement(
		Segment &segment,bool reverse, blocks::index_t position)
	:
		segment_(segment), reverse_(reverse),
		startPosInPath_(position), endPosInPath_(position + segment.distance())
{}

PathElement::~PathElement() {}

std::ostream &operator <<(std::ostream &os, const PathElement &e)
{
	os << e.startPos() << ": " << Current(&e.segment(), e.reverse());
	return os;
}

inline blocks::index_t PathElement::nextRepeater_internal(blocks::index_t relPos) const
{
	auto relResult = segment_.get().nextRepeater(relPos, reverse_);

	if (relResult < 0)
		return relResult;

	return relResult + startPosInPath_;
}

blocks::index_t PathElement::nextRepeater() const
{
	return nextRepeater_internal(0);
}

blocks::index_t PathElement::nextRepeater(blocks::index_t pos) const
{
	return nextRepeater_internal(pos - startPosInPath_);
}

} /* namespace rhdl::spatial */
