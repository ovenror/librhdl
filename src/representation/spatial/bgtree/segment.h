#ifndef WIRESEGMENT_H
#define WIRESEGMENT_H

#include "representation/blocks/blocks.h"

#include "util/iterable.h"

#include <vector>
#include <set>

namespace rhdl {
namespace TM {

class UniqueSegment;
class Segment;
class Connector;

using UniqueCurrent = std::pair<UniqueSegment *, bool>;

/*
 * Segment with flow direction.
 * 		second = reverse
 * 			false: from lower to higher position
 * 			true: from higher to lower position
 */
using Current = std::pair<Segment *, bool>;

class Segment
{
public:
	Segment(blocks::Blocks::index_t start, blocks::Blocks::index_t end);

	virtual blocks::Blocks::index_t start() const {return start_;}
	virtual blocks::Blocks::index_t end() const {return end_;}

	blocks::Blocks::index_t distance() const {return end_ - start_;}
	blocks::Blocks::index_t size() const {return distance() - 1;}

	virtual const Connector &frontConnector() const = 0;
	virtual const Connector &backConnector() const = 0;

	const Connector &connector(bool back) const;
	bool terminal(bool back) const;

	virtual const UniqueSegment &firstUnique() const = 0;
	virtual const UniqueSegment &lastUnique() const = 0;

	void placeRepeater(blocks::Blocks::index_t relPos, bool reverse);
	blocks::Blocks::index_t nextRepeater(blocks::Blocks::index_t from, bool reverse) const;

	blocks::Blocks::index_t firstRepeaterOffset() const;
	blocks::Blocks::index_t lastRepeaterOffset() const;
	blocks::Blocks::index_t repeaterOffset(bool back) const;
	blocks::Blocks::index_t repeaterSpace() const;

protected:
	friend std::ostream &operator<<(std::ostream &os, const Segment &segment);

	virtual bool noFrontCrossConnections() const = 0;
	virtual bool noBackCrossConnections() const = 0;
	virtual void placeRepeaterAbs(blocks::Blocks::index_t absPos, bool reverse) = 0;
	virtual void addToStream(std::ostream &os) const = 0;

	const blocks::Blocks::index_t start_;
	const blocks::Blocks::index_t end_;
	mutable std::set<blocks::Blocks::index_t> repeaters_;
};

std::ostream &operator<<(std::ostream &os, const Segment &segment);
std::ostream &operator<<(std::ostream &os, const UniqueCurrent &current);
std::ostream &operator<<(std::ostream &os, const Current &current);

}
}

#endif // CURRENT_H
