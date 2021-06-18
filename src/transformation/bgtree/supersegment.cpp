#include "supersegment.h"
#include "uniquesegment.h"
#include "connector.h"
#include "wire.h"

namespace rhdl::TM {

using blocks::Blocks;

SuperSegment::SuperSegment(const std::vector<const UniqueSegment *> &segments)
	: Segment(segments.front()->start(), segments.back()->end()),
	  first_(*segments.front()), last_(*segments.back())
{
	assert (segments.size() >= 2);

	Blocks::index_t lastEnd = start_;
	for (const UniqueSegment *segment : segments) {
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

void SuperSegment::placeRepeaterAbs(Blocks::index_t absPos, bool reverse, Blocks &b) const
{
	auto iter = parts_.lower_bound(absPos);
	assert (iter != parts_.end());
	const UniqueSegment &containing = **iter;
	containing.placeRepeaterAbs(absPos, reverse, b);
}

void SuperSegment::addToStream(std::ostream &os) const
{
	os << (firstUnique().wire().vertical() ? "-" : "|");
	os << " S FROM: " << frontConnector() << " TO: " << backConnector();
}

bool SuperSegment::PartLess::operator()(const UniqueSegment * const &lhs, const UniqueSegment * const &rhs) const
{
	return lhs -> end() < rhs -> end();
}

bool SuperSegment::PartLess::operator()(const UniqueSegment * const &lhs, const Blocks::index_t &rhs) const
{
	return lhs -> end() < rhs;
}

bool SuperSegment::PartLess::operator()(const Blocks::index_t &lhs, const UniqueSegment * const &rhs) const
{
	return lhs < rhs -> end();
}

}

