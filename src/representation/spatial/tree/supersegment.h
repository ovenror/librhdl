#ifndef SUPERSEGMENT_H
#define SUPERSEGMENT_H

#include <vector>
#include <set>
#include "../tree/segment.h"

namespace rhdl {
namespace spatial {

class AtomicSegment;
class Connector;

class SuperSegment : public Segment
{
public:
	SuperSegment(const std::vector<AtomicSegment *> &segments);

	const Connector &frontConnector() const override;
	const Connector &backConnector() const override;

	const AtomicSegment &firstUnique() const override {return first_;}
	const AtomicSegment &lastUnique() const override {return last_;}

protected:
	bool noFrontCrossConnections() const override;
	bool noBackCrossConnections() const override;
	void placeRepeaterAbs(blocks::index_t absPos, bool reverse) override;
	virtual void addToStream(std::ostream &os) const override;

private:
	struct PartLess {
		using is_transparent = void;

		bool operator()(const AtomicSegment* const &lhs, const AtomicSegment* const &rhs) const;
		bool operator()(const AtomicSegment* const &lhs, const blocks::index_t &rhs) const;
		bool operator()(const blocks::index_t &lhs, const AtomicSegment* const &rhs) const;
	};

	const AtomicSegment &first_;
	const AtomicSegment &last_;
	std::set<AtomicSegment *, PartLess> parts_;
};

}
}

#endif // SUPERSEGMENT_H
