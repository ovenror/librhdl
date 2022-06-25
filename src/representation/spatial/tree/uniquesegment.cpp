#include "uniquesegment.h"
#include "connector.h"
#include "supersegment.h"
#include "wire.h"

#include "representation/blocks/types.h"

namespace rhdl::spatial {

UniqueSegment::UniqueSegment(
		Wire &wire, blocks::index_t start, blocks::index_t end,
		Connector &front, Connector &back)

	: Segment(start, end), wire_(wire), front_(front), back_(back)
{
	front.addOutgoing (*this);
	back.addIncoming(*this);
}

Segment &UniqueSegment::super() {
	if (super_) {
		assert (&super_ -> firstUnique() != &super_ -> lastUnique());
		return *super_;
	}
	else {
		return *this;
	}
}

void UniqueSegment::placeRepeaterAbs(blocks::index_t absPos, bool reverse)
{
	wire_.addRepeater(absPos, reverse);
}

UniqueSegment *UniqueSegment::straightAfter() const
{
	return back_.straightPartner(*this);
}

bool UniqueSegment::noFrontCrossConnections() const
{
	return frontConnector().terminal() || straightBefore();
}

bool UniqueSegment::noBackCrossConnections() const
{
	return backConnector().terminal() || straightAfter();
}

UniqueSegment *UniqueSegment::straightBefore() const
{
	return front_.straightPartner(*this);
}

void UniqueSegment::setSuper(const std::shared_ptr<SuperSegment> &super) const
{
	assert (!super_);
	super_ = super;
}

void UniqueSegment::addToStream(std::ostream &os) const
{
	os << (wire_.vertical() ? "-" : "|");
	os << " " << start_ << "-" << end_;
	os << " @" << wire_;
}

}
