#include "../tree/segment.h"

#include "../tree/connection.h"
#include "../tree/connector.h"
#include "../tree/node.h"
#include "../tree/supersegment.h"
#include "../tree/uniquesegment.h"
#include "../tree/wire.h"

namespace rhdl::spatial {

std::ostream &operator<<(std::ostream &os, const Segment &segment)
{
	os << segment.distance() << ": ";
	segment.addToStream(os);
	return os;
}

static std::ostream &current_out(std::ostream &os, const Segment *segment, bool reverse)
{
	os << (reverse ? "-" : "+") << *segment;
	return os;
}

std::ostream &operator<<(std::ostream &os, const UniqueCurrent &current)
{
	return current_out(os, current.first, current.second);
}

std::ostream &operator<<(std::ostream &os, const Current &current)
{
	return current_out(os, current.first, current.second);
}

const Connector &Segment::connector(bool back) const
{
	return back ? backConnector() : frontConnector();
}

bool Segment::terminal(bool back) const
{
	return connector(back).terminal();
}

void Segment::placeRepeater(blocks::index_t relPos, bool reverse)
{
	assert (relPos >= 0 && relPos <= distance());

	blocks::index_t absPos;

	if (reverse)
		absPos = end() - relPos;
	else
		absPos = start() + relPos;

	repeaters_.insert(absPos - start());

	//std::cerr << "placeRepeater r:" << relPos << ", a:" << absPos << ", " << reverse << "@" << *this << std::endl;

	placeRepeaterAbs(absPos, reverse);
}

blocks::index_t Segment::nextRepeater(blocks::index_t from, bool reverse) const
{
	assert (from >= 0 && from <= distance());

	decltype(repeaters_)::iterator iter;

	if (reverse) {
		from = distance() - from;
		iter = repeaters_.upper_bound(from);
		if (iter == repeaters_.begin())
			return -1;
		--iter;
		return distance() - *iter;
	}
	else {
		iter = repeaters_.lower_bound(from);
		if (iter == repeaters_.end())
			return -1;
		return *iter;
	}
}

blocks::index_t Segment::firstRepeaterOffset() const
{
	if (!firstUnique().wire().vertical()) {
		return 2;
	}

	if (!noFrontCrossConnections())
		return 1;

	if (firstUnique().wire().isInverterOutput())
		return 1;

	return 0;
}

blocks::index_t Segment::lastRepeaterOffset() const
{
	if (!lastUnique().wire().vertical()) {
		return 2;
	}

	if (!noBackCrossConnections())
		return 1;

	const Wire &w = lastUnique().wire();
	const Node *inv = w.getInverter();

	if (!inv || !w.isInputOfInverter(inv))
		return 0;

	if (inv -> shortcut_)
		return 2;
	else
		return 1;
}

blocks::index_t Segment::repeaterOffset(bool back) const
{
	return back ? lastRepeaterOffset() : firstRepeaterOffset();
}

blocks::index_t Segment::repeaterSpace() const
{
	blocks::index_t result = distance() + 1 - firstRepeaterOffset() - lastRepeaterOffset();
	return result > 0 ? result : 0;
}

Segment::Segment(blocks::index_t start, blocks::index_t end)
	: start_(start), end_(end)
{}

}
