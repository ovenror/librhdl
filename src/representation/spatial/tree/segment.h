#ifndef WIRESEGMENT_H
#define WIRESEGMENT_H

#include "representation/blocks/types.h"

#include "util/iterable.h"

#include <vector>
#include <set>

namespace rhdl {
namespace spatial {

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
	Segment(blocks::index_t start, blocks::index_t end);

	virtual blocks::index_t start() const {return start_;}
	virtual blocks::index_t end() const {return end_;}

	blocks::index_t distance() const {return end_ - start_;}
	blocks::index_t size() const {return distance() - 1;}

	virtual const Connector &frontConnector() const = 0;
	virtual const Connector &backConnector() const = 0;

	const Connector &connector(bool back) const;
	bool terminal(bool back) const;

	virtual const UniqueSegment &firstUnique() const = 0;
	virtual const UniqueSegment &lastUnique() const = 0;

	void placeRepeater(blocks::index_t relPos, bool reverse);
	blocks::index_t nextRepeater(blocks::index_t from, bool reverse) const;

	blocks::index_t firstRepeaterOffset() const;
	blocks::index_t lastRepeaterOffset() const;
	blocks::index_t repeaterOffset(bool back) const;
	blocks::index_t getOnewayRepeatersStart() const;
	blocks::index_t getOnewayRepeatersBackOffset() const;
	blocks::index_t getOnewayRepeatersEnd() const;
	blocks::index_t globalRepeaterPositionIdxToLocalOffset(unsigned int) const;
	blocks::index_t repeaterSpace() const;

	bool isOneway() const;
	bool getOnewayReverse() const;

	void useDirection(bool reverse);

	void setGlobalRepeaterPositionsEndIdx(unsigned int);
	bool hasGlobalRepeaterPositions() const;
	unsigned int getGlobalRepeaterPositionsStartIdx() const;

protected:
	friend std::ostream &operator<<(std::ostream &os, const Segment &segment);
	bool hasOnewayDirection() const;
	void setOnewayDirection(bool reverse);
	void setBiDirectional();
	void assertDirection(bool reverse) const;

	unsigned int getGlobalRepeaterPositionsEndIdx() const;

	virtual bool noFrontCrossConnections() const = 0;
	virtual bool noBackCrossConnections() const = 0;
	virtual void placeRepeaterAbs(blocks::index_t absPos, bool reverse) = 0;
	virtual void addToStream(std::ostream &os) const = 0;

	mutable std::set<blocks::index_t> repeaters_;
	const blocks::index_t start_;
	const blocks::index_t end_;
	unsigned int globalRepeaterPositionsEndIdx_;
	bool hasGlobalRepeaterPositions_ = false;
	bool oneway_ = true;
	bool hasOnewayDirection_ = false;
	bool onewayReverse_ = false;
};

std::ostream &operator<<(std::ostream &os, const Segment &segment);
std::ostream &operator<<(std::ostream &os, const UniqueCurrent &current);
std::ostream &operator<<(std::ostream &os, const Current &current);

}
}

#endif // CURRENT_H
