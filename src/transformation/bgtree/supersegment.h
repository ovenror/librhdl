#ifndef SUPERSEGMENT_H
#define SUPERSEGMENT_H

#include "segment.h"

#include <vector>
#include <set>

namespace rhdl {
namespace TM {

class UniqueSegment;
class Connector;

class SuperSegment : public Segment
{
public:
	SuperSegment(const std::vector<const UniqueSegment *> &segments);

	const Connector &frontConnector() const override;
	const Connector &backConnector() const override;

	const UniqueSegment &firstUnique() const override {return first_;}
	const UniqueSegment &lastUnique() const override {return last_;}

protected:
	bool noFrontCrossConnections() const override;
	bool noBackCrossConnections() const override;
	void placeRepeaterAbs(Blocks::index_t absPos, bool reverse, Blocks &b) const override;
	virtual void addToStream(std::ostream &os) const override;

private:
	struct PartLess {
		using is_transparent = void;

		bool operator()(const UniqueSegment* const &lhs, const UniqueSegment* const &rhs) const;
		bool operator()(const UniqueSegment* const &lhs, const Blocks::index_t &rhs) const;
		bool operator()(const Blocks::index_t &lhs, const UniqueSegment* const &rhs) const;
	};

	const UniqueSegment &first_;
	const UniqueSegment &last_;
	std::set<const UniqueSegment *, PartLess> parts_;
};

}
}

#endif // SUPERSEGMENT_H