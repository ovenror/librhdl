#include "uniquesegment.h"
#include "connector.h"
#include "supersegment.h"
#include "wire.h"

namespace rhdl::TM {

UniqueSegment::UniqueSegment(
		const Wire &wire, Blocks::index_t start, Blocks::index_t end,
		Connector &front, Connector &back)

	: Segment(start, end), wire_(wire), front_(front), back_(back)
{
	front.addOutgoing (*this);
	back.addIncoming(*this);
}

const Segment &UniqueSegment::super() const {
	if (super_) {
		assert (&super_ -> firstUnique() != &super_ -> lastUnique());
		return *super_;
	}
	else {
		return *this;
	}
}

void UniqueSegment::placeRepeaterAbs(Blocks::index_t absPos, bool reverse, Blocks &b) const
{
	wire_.placeRepeater(absPos, reverse, b);
}

const UniqueSegment *UniqueSegment::straightAfter() const
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

const UniqueSegment *UniqueSegment::straightBefore() const
{
	return front_.straightPartner(*this);
}

void UniqueSegment::setSuper(const std::shared_ptr<const SuperSegment> &super) const
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
