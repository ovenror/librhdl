#ifndef UNIQUESEGMENT_H
#define UNIQUESEGMENT_H

#include "segment.h"

#include "representation/blocks/blocks.h"

namespace rhdl {
namespace TM {

class Wire;
class Connector;
class SuperSegment;

class UniqueSegment : public Segment
{
public:   
	UniqueSegment(
			Wire &wire, blocks::Blocks::index_t start, blocks::Blocks::index_t end,
			Connector &frontConnector, Connector &backConnector);

	const Wire &wire() const {return wire_;}

	const Connector &frontConnector() const override {return front_;}
	const Connector &backConnector() const override {return back_;}

	const UniqueSegment &firstUnique() const override {return *this;}
	const UniqueSegment &lastUnique() const override {return *this;}

	Segment &super();

	void setSuper(const std::shared_ptr<SuperSegment> &super) const;
	bool hasSuper() const {return super_ != nullptr;}

	UniqueSegment *straightBefore() const;
	UniqueSegment *straightAfter() const;

	void placeRepeaterAbs(blocks::Blocks::index_t absPos, bool reverse) override;

protected:
	bool noFrontCrossConnections() const override;
	bool noBackCrossConnections() const override;
	void addToStream(std::ostream &os) const override;

private:
	friend bool operator<(blocks::Blocks::index_t lhs, const UniqueSegment &rhs);

	Wire &wire_;

	const Connector &front_;
	const Connector &back_;

	mutable std::shared_ptr<SuperSegment> super_;
};

}
}

#endif // UNIQUESEGMENT_H
