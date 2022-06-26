#include "atomicsegment.h"

#include "connector.h"
#include "supersegment.h"
#include "wire.h"

#include "representation/blocks/types.h"

namespace rhdl::spatial {

AtomicSegment::AtomicSegment(
		Wire &wire, blocks::index_t start, blocks::index_t end,
		Connector &front, Connector &back)

	: Segment(start, end), wire_(wire), front_(front), back_(back)
{
	front.addOutgoing (*this);
	back.addIncoming(*this);
}

Segment &AtomicSegment::super() {
	if (super_) {
		assert (&super_ -> firstUnique() != &super_ -> lastUnique());
		return *super_;
	}
	else {
		return *this;
	}
}

void AtomicSegment::placeRepeaterAbs(blocks::index_t absPos, bool reverse)
{
	wire_.addRepeater(absPos, reverse);
}

AtomicSegment *AtomicSegment::straightAfter() const
{
	return back_.straightPartner(*this);
}

bool AtomicSegment::noFrontCrossConnections() const
{
	return frontConnector().terminal() || straightBefore();
}

bool AtomicSegment::noBackCrossConnections() const
{
	return backConnector().terminal() || straightAfter();
}

AtomicSegment *AtomicSegment::straightBefore() const
{
	return front_.straightPartner(*this);
}

void AtomicSegment::setSuper(const std::shared_ptr<SuperSegment> &super) const
{
	assert (!super_);
	super_ = super;
}

void AtomicSegment::addToStream(std::ostream &os) const
{
	os << (wire_.vertical() ? "-" : "|");
	os << " " << start_ << "-" << end_;
	os << " @" << wire_;
}

}
