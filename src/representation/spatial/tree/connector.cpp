#include "../tree/connector.h"

#include "../tree/node.h"
#include "../tree/wire.h"
#include "atomicsegment.h"

namespace rhdl::spatial {

Connector::Connector(blocks::index_t xpos, blocks::index_t ypos)
	: xpos_(xpos), ypos_(ypos), terminal_(true)
{}

Connector::SuperCurrentIterable Connector::superConnected() const
{
	Super super = [](const AtomicCurrent &c){return Current(&c.first -> super(), c.second);};

	auto begin = boost::make_transform_iterator(connected_.begin(), super);
	auto end = boost::make_transform_iterator(connected_.end(), super);

	return SuperCurrentIterable(begin, end);
}

blocks::index_t Connector::getPositionOn(const Wire &w) const
{
	return w.vertical() ? ypos_ : xpos_;
}

AtomicSegment *Connector::straightPartner(const AtomicSegment &segment) const
{
	assert (!segment.wire().anchor_);
	AtomicSegment *result = nullptr;
	const Wire &wSegment = segment.wire();

	for (const AtomicCurrent &c : connected()) {
		AtomicSegment *partner = c.first;
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

void Connector::add(AtomicCurrent &&c)
{
	checkPathTerminality(c);
	connected_.push_back(c);
}

bool Connector::breaksTerminality(const AtomicCurrent &c) const
{
	const AtomicSegment *s = c.first;

	if (s -> wire().anchor_)
		return false;

	for (const AtomicCurrent &c : connected()) {
		const AtomicSegment *segment = c.first;

		if (!segment -> wire().anchor_)
			return true;
	}

	return false;
}


bool Connector::terminal() const
{
	return terminal_;

#if 0
	const AtomicSegment *found = nullptr;
	for (const Current &c : connected()) {
		const AtomicSegment *segment = c.first;

		if (segment -> wire().anchor_)
			continue;

		if (found)
			return false;

		found = segment;
	}

	return true;
#endif
}

void Connector::addIncoming(AtomicSegment &segment)
{
	add({&segment, true});
}

void Connector::addOutgoing(AtomicSegment &segment)
{
	add({&segment, false});
}

void Connector::checkPathTerminality(const AtomicCurrent &c)
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
