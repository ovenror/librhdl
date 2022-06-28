/*
 * path.cpp
 *
 *  Created on: Jun 25, 2022
 *      Author: ovenror
 */

#include "path.h"
#include <cassert>

namespace rhdl {
namespace spatial {

Path::Path(Container elements)
		: elements_(std::move(elements)),
		  length_(elements_.empty() ? 0 : elements_.back().endPos() + 1)
{}

Path::~Path() {}

void Path::find(blocks::index_t start, const_iterator &e) const
{
	assert (e -> startPos() <= start);

	for (; e != cend(); ++e) {
		if (e -> endPos() > start)
			break;
	}

	assert (e != cend() || start == elements_.back().endPos());
}

blocks::index_t Path::nextRepeater(
		blocks::index_t start, const_iterator &e) const
{
	find(start, e);

	if (e == cend())
		return -1;

	blocks::index_t nextRep = e -> nextRepeater(start);

	while (nextRep < 0) {
		if (++e == cend())
			return -1;

		nextRep = e -> nextRepeater();
	}

	return nextRep;
}

blocks::index_t Path::freeLength(blocks::index_t start, const_iterator &curIter) const
{
	auto nextRep = nextRepeater(start, curIter);
	auto stop = nextRep >= 0 ? nextRep : length_;
	return stop - start;
}

blocks::index_t Path::freeLength(blocks::index_t start) const
{
	auto from_first_element = begin();
	return freeLength(start, from_first_element);
}

std::ostream &operator<<(std::ostream &os, SplitPathRating &rating)
{
	os << "(" << rating.first << "," << rating.second << ")";
	return os;
}

std::ostream &operator<<(std::ostream &os, PositionRating &rating)
{
	os << "(" << rating[0] << "," << rating[1] << ")";
	return os;
}

void Path::evaluatePositions(std::vector<PositionRating> &result) const
{
	blocks::index_t freeLen;
	blocks::index_t positionInPath = 0;
	blocks::index_t skipTo;
	blocks::index_t stopAt;

	blocks::index_t posIdx;

	//std::cerr << "Evaluating path " << std::endl << *this << std::endl;

	Segment *segment;
	bool reverse;
	auto pathElement = begin();

	while (true) {
		assert (pathElement != cend());
		skipTo = positionInPath;
		auto freeIter = pathElement;
		freeLen = freeLength(skipTo, freeIter);

		while (freeLen <= redstone::maxWireLength) {
			skipTo += freeLen + 1;

			assert ((skipTo >= length_) == (freeIter == cend()));

			if (skipTo >= length_)
				return;

			assert (skipTo <= freeIter -> endPos());

			pathElement = freeIter;

			if (skipTo == freeIter -> endPos())
				++pathElement;

			assert ((pathElement == cend()) == (skipTo == length_ -1));

			if (skipTo == length_ - 1) {
				freeLen = 1;
				pathElement = freeIter;
				break;
			}

			freeLen = freeLength(skipTo, freeIter);

			assert (skipTo + freeLen <= length_);
		}

		positionInPath = pathElement -> startPos();
		stopAt = skipTo + freeLen;
		assert (stopAt <= length_);

		for (; pathElement != end(); ++ pathElement) {
			segment = &pathElement -> segment();
			reverse = pathElement -> reverse();

			//std::cerr << "  next seg "<< std::endl;

			blocks::index_t endPosition = pathElement -> endPos();

			assert (endPosition >= skipTo);

			if (!segment -> hasGlobalRepeaterPositions())
			{
				positionInPath = endPosition;
				continue;
			}

			positionInPath += segment -> repeaterOffset(reverse);

			if (positionInPath >= stopAt) {
				positionInPath = stopAt;
				break;
			}

			blocks::index_t skip = skipTo - positionInPath;

			assert (positionInPath <= stopAt);

			if (positionInPath == stopAt)
				break;

			if (skip < 0)
				skip = 0;

			blocks::index_t nUnskipped = segment -> repeaterSpace() - skip;

			if (nUnskipped < 0) {
				if (stopAt < endPosition) {
					positionInPath = stopAt;
					break;
				}

				positionInPath = endPosition;
				continue;
			}

			positionInPath += skip;
			int nIterations = std::min(stopAt - positionInPath, nUnskipped);

			unsigned int allStartIdx = segment -> getGlobalRepeaterPositionsStartIdx();
			unsigned int startIdx = allStartIdx + skip;
			unsigned int endIdx = startIdx + nIterations;

			for (posIdx = startIdx; posIdx < endIdx; ++posIdx) {
				//std::cerr << repeaterPosition << ",";
				auto rating = rate(freeLen, positionInPath++ - skipTo);
				//std::cerr << posIdx << ": " << segment << ", ";
				//std::cerr << (posIdx - (posIter -> second) + (segment -> repeaterOffset(reverse)));
				//std::cerr << ": " << rating << std::endl;
				result[posIdx] = rating;
			}

			assert (positionInPath <= stopAt);

			positionInPath += segment -> repeaterOffset(!reverse) - 1;

			if (positionInPath >= stopAt)
				positionInPath = stopAt;

			if (positionInPath == stopAt)
				break;
		}

		assert (positionInPath <= length_);

		if (pathElement == cend() || positionInPath == length_)
			break;

		++positionInPath;
	}

	//std::cerr << "end" << std::endl;
}

} /* namespace spatial */
} /* namespace rhdl */
