#include "connector.h"
#include "uniquesegment.h"
#include "wire.h"
#include "node.h"

namespace rhdl::TM {

using blocks::Blocks;

Connector::Connector(Blocks::index_t xpos, Blocks::index_t ypos)
	: xpos_(xpos), ypos_(ypos), terminal_(true)
{}

Connector::SuperCurrentIterable Connector::superConnected() const
{
	Super super = [](const UniqueCurrent &c){return Current(&c.first -> super(), c.second);};

	auto begin = boost::make_transform_iterator(connected_.begin(), super);
	auto end = boost::make_transform_iterator(connected_.end(), super);

	return SuperCurrentIterable(begin, end);
}

Blocks::index_t Connector::getPositionOn(const Wire &w) const
{
	return w.vertical() ? ypos_ : xpos_;
}

UniqueSegment *Connector::straightPartner(const UniqueSegment &segment) const
{
	assert (!segment.wire().anchor_);
	UniqueSegment *result = nullptr;
	const Wire &wSegment = segment.wire();

	for (const UniqueCurrent &c : connected()) {
		UniqueSegment *partner = c.first;
		const Wire &wPartner = partner -> wire();

		if (partner == &segment)
			continue;

		assert (!wPartner.anchor_);

		if (wPartner.vertical() != wSegment.vertical())
			return nullptr;

		assert (!result);
		result = partner;
	}

	return result;
}

void Connector::add(UniqueCurrent &&c)
{
	checkPathTerminality(c);
	connected_.push_back(c);
}

bool Connector::breaksTerminality(const UniqueCurrent &c) const
{
	const UniqueSegment *s = c.first;

	if (s -> wire().anchor_)
		return false;

	for (const UniqueCurrent &c : connected()) {
		const UniqueSegment *segment = c.first;

		if (!segment -> wire().anchor_)
			return true;
	}

	return false;
}


bool Connector::terminal() const
{
	return terminal_;

#if 0
	const UniqueSegment *found = nullptr;
	for (const Current &c : connected()) {
		const UniqueSegment *segment = c.first;

		if (segment -> wire().anchor_)
			continue;

		if (found)
			return false;

		found = segment;
	}

	return true;
#endif
}

void Connector::addIncoming(UniqueSegment &segment)
{
	add({&segment, true});
}

void Connector::addOutgoing(UniqueSegment &segment)
{
	add({&segment, false});
}

void Connector::checkPathTerminality(const UniqueCurrent &c)
{
	if (!terminal_)
		return;

	terminal_ = !breaksTerminality(c);
}

std::ostream &operator<<(std::ostream &os, const Connector &segment)
{
	auto pos = segment.position();
	os << "Connector: (" << pos.first << "," << pos.second << ")";
	return os;
}

}
