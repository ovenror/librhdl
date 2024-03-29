#include "../tree/supersegment.h"

#include "../tree/connector.h"
#include "../tree/wire.h"
#include "atomicsegment.h"

namespace rhdl::spatial {

SuperSegment::SuperSegment(const std::vector<AtomicSegment *> &segments)
	: Segment(segments.front()->start(), segments.back()->end()),
	  first_(*segments.front()), last_(*segments.back())
{
	assert (segments.size() >= 2);

	blocks::index_t lastEnd = start_;
	for (AtomicSegment *segment : segments) {
		assert (segment -> start() == lastEnd);
		parts_.insert(segment);
		lastEnd = segment -> end();
	}
}

const Connector &SuperSegment::frontConnector() const
{
	return firstUnique().frontConnector();
}

const Connector &SuperSegment::backConnector() const
{
	return lastUnique().backConnector();
}

bool SuperSegment::noFrontCrossConnections() const
{
	return frontConnector().terminal();
}

bool SuperSegment::noBackCrossConnections() const
{
	return frontConnector().terminal();
}

void SuperSegment::placeRepeaterAbs(blocks::index_t absPos, bool reverse)
{
	auto iter = parts_.lower_bound(absPos);
	assert (iter != parts_.end());
	AtomicSegment &containing = **iter;
	containing.placeRepeaterAbs(absPos, reverse);

	if (++iter == parts_.end())
		return;

	AtomicSegment &next = **iter;
	assert (next.start() >= absPos);

	if (next.start() == absPos)
		next.placeRepeaterAbs(absPos, reverse);
}

void SuperSegment::addToStream(std::ostream &os) const
{
	os << (firstUnique().wire().vertical() ? "-" : "|");
	os << " S FROM: " << frontConnector() << " TO: " << backConnector();
}

bool SuperSegment::PartLess::operator()(const AtomicSegment * const &lhs, const AtomicSegment * const &rhs) const
{
	return lhs -> end() < rhs -> end();
}

bool SuperSegment::PartLess::operator()(const AtomicSegment * const &lhs, const blocks::index_t &rhs) const
{
	return lhs -> end() < rhs;
}

bool SuperSegment::PartLess::operator()(const blocks::index_t &lhs, const AtomicSegment * const &rhs) const
{
	return lhs < rhs -> end();
}

}

