#include "../tree/fixoverlongwires_impl.h"

#include "representation/blocks/blocks.h"

#include "redstone.h"

#include <functional>
#include <memory>
#include <limits>
#include "../tree/connector.h"
#include "../tree/layer.h"
#include "../tree/node.h"
#include "../tree/supersegment.h"
#include "../tree/uniquesegment.h"
#include "../tree/wire.h"

namespace rhdl::TM {

using blocks::Blocks;

void eraseWorkingLinks(std::map<Link, Paths> &paths)
{
	for (auto iter = paths.begin(); iter != paths.end();) {
		const Paths &thePaths = iter -> second;

		bool fix0red = false;
		for (const auto &ppath : thePaths) {
			if (fixed(*ppath)) {
				fix0red = true;
				break;
			}
		}

		if (fix0red)
			iter = paths.erase(iter);
		else
			++iter;
	}
}

FixConnectionResult fixConnection(const Links &links)
{
	std::map<Link, Paths> paths = findPaths(links);

	for (const auto &kv : paths)
		assert (kv.second.size() == 1);

	std::map<Segment *, bool> currents =
			identifyEligibleCurrents(paths);

	SegmentToPositionIndex segmentToPositionIdx;
	unsigned int nPositions = makePositionMap(currents, segmentToPositionIdx);

	eraseWorkingLinks(paths);

	if (paths.empty())
		return FixConnectionResult::UNCHANGED;

	RepeaterPlacement bestPosition;

	while (!paths.empty()) {
		bestPosition = findBestPlacement(paths, currents, segmentToPositionIdx, nPositions);

		if (bestPosition.first.first == nullptr)
			return FixConnectionResult::BROKEN;

		placeRepeater(bestPosition);
		eraseWorkingLinks(paths);
	}

	return FixConnectionResult::FIXED;
}

std::map<Link, Paths> findPaths(const Links &links)
{
	std::map<Link, Paths> result;

	for (const Link &link : links)
		result[link] = findPaths(link);

	return result;
}

Paths findPaths(const Link &link)
{
	const Connector *sStart = link.first;
	const Connector *sEnd = link.second;

	//std::cerr << "calculating paths from " << std::endl;
	//std::cerr << "    " << *sStart << std::endl;
	//std::cerr << "to" << std::endl;
	//std::cerr << "    " << *sEnd << std::endl;

	if (sStart == sEnd) {
		Paths paths;
		paths.push_back(std::make_unique<Path>(Path()));
		return paths;
	}

	Paths allPaths;
	std::vector<std::pair<Path, const Connector *>> lastPaths = {{{}, sStart}};
	std::vector<std::pair<Path, const Connector *>> currentPaths;

	for (unsigned int len = 1; !lastPaths.empty(); ++len) {
		for (const auto &last : lastPaths) {
			const Path &lastPath = last.first;
			const Connector *lastSharedSegment = last.second;

			assert (lastPath.size() == len - 1);

			//std::cerr << "extending..." << std::endl;
			//std::cerr << lastPath << std::endl;

			for (const Current &current : lastSharedSegment -> superConnected()) {
				const Segment *segment = current.first;

				//std::cerr << "  trying..." << std::endl;
				//std::cerr << "  " << current << std::endl;

				if (std::any_of(
							lastPath.begin(), lastPath.end(),
							[segment](const Current &c)
							{
								return c.first == segment;
							}
							))
					continue;

				//std::cerr << "  new..." << std::endl;

				const Connector &nextSharedSegment = current.second ?
							segment -> frontConnector() : segment -> backConnector();

				if (&nextSharedSegment == sEnd) {
					//std::cerr << "   !!!FOUND PATH!!!" << std::endl;
					allPaths.push_back(std::make_unique<Path>(lastPath));
					allPaths.back() -> push_back(current);
				}
				else {
					//std::cerr << "   ready for next round" << std::endl;
					currentPaths.emplace_back(lastPath, &nextSharedSegment);
					currentPaths.back().first.push_back(current);
				}
			}
		}

		lastPaths = std::move(currentPaths);
		currentPaths = {};
	}

	assert (!allPaths.empty());

	return allPaths;
}

const std::map<Segment *, bool> identifyEligibleCurrents(
		const std::map<Link, Paths> &paths)
{
	std::set<Segment *> bidirectionalSegments;
	std::map<Segment *, bool> result;

	for (const auto &kv : paths) {
		for (const auto &ppath : kv.second) {
			for (const Current &current : *ppath) {
				Segment *segment = current.first;
				bool reverse = current.second;
				std::map<Segment *, bool>::iterator iter;

				if (bidirectionalSegments.find(segment) != bidirectionalSegments.end())
					continue;

				if ((iter = result.find(segment)) == result.end()) {
					result[segment] = reverse;
					continue;
				}

				if (iter -> second == reverse)
					continue;

				result.erase(iter);
				bidirectionalSegments.insert(iter -> first);
			}
		}
	}

	return result;
}

std::ostream &operator<<(std::ostream &os, const Path &path)
{
	os << "PATH" << std::endl;

	for (const Current &c: path) {
		os << "  " << c << std::endl;
	}

	return os;
}

const Connector &getOutputConnector(const Node &node)
{
	const Wire &wire = node.output_;
	assert (!node.backwards());
	assert (wire.hasSegments());
	return wire.front();
}

const Connector &getInputConnector(const Node &node)
{
	const Wire &wire = node.input_;
	assert (!node.backwards());
	assert (wire.hasSegments());
	return wire.back();
}

const Connector &getIFaceConnector(const Wire &ifaceWire)
{
	return ifaceWire.getLayer() ? ifaceWire.back() : ifaceWire.front();
}

unsigned int makePositionMap(
		const std::map<Segment *, bool> &eligible,
		SegmentToPositionIndex &result)
{
	SegmentToPositionIndex::left_map &left = result.left;
	unsigned int idx = 0;

	for (const Current &current : eligible) {
		Segment *segment = current.first;
		Blocks::index_t space = segment -> repeaterSpace();

		if (space <= 0)
			continue;

		//std::cerr << "adding " << *segment << " to map: " << idx;

		left.insert(SegmentToPositionIndex::left_value_type(segment, idx));
		idx += space;
		//std::cerr << "--" << idx-1 << std::endl;
	}

	return idx;
}

RepeaterPlacement findBestPlacement(const std::map<Link, Paths> &paths,
		const std::map<Segment *, bool> &currents,
		const SegmentToPositionIndex &map,
		unsigned int nPositions)
{    
	std::vector<TotalPositionRating> evaluatedPositions(nPositions);

	evaluatePositions(paths, map, evaluatedPositions);
	return findBestPlacement(currents, map, evaluatedPositions);
}

RepeaterPlacement findBestPlacement(const std::map<Segment *, bool> &currents,
		const SegmentToPositionIndex &map,
		const std::vector<TotalPositionRating> &evaluatedPositions)
{
	TotalPositionResult dummyResult;
	RepeaterPlacement bestPosition{{nullptr, false}, -1};
	const TotalPositionRating *bestRating = &dummyResult.rating_;

	for (unsigned int posIdx = 0; posIdx < evaluatedPositions.size(); ++posIdx) {
		const TotalPositionRating &posRating = evaluatedPositions[posIdx];

		if (!std::lexicographical_compare(
					bestRating -> begin(), bestRating -> end(),
					posRating.begin(), posRating.end()
				))
			continue;

		RepeaterPlacement placement = getPlacementFromIdx(posIdx, currents, map);

		bestPosition = placement;
		bestRating = &posRating;
	}

	return bestPosition;
}

RepeaterPlacement getPlacementFromIdx(unsigned int positionIdx,
		const std::map<Segment *, bool> &currents,
		const SegmentToPositionIndex &map)
{
	Blocks::index_t distance = 0;
	SegmentToPositionIndex::right_const_iterator iter;

	while ((iter = map.right.find(positionIdx)) == map.right.end()) {
		--positionIdx;
		++distance;
	}

	Segment *segment = iter -> second;
	bool reverse = currents.at(segment);

	distance += segment -> repeaterOffset(reverse);

	return {{segment, reverse}, distance};
}

void evaluatePositions(
		const std::map<Link, Paths> &paths,
		const SegmentToPositionIndex &map,
		std::vector<TotalPositionRating> &result)
{
	constexpr unsigned int invertersNeededCutoff = 10;

	for (const auto &kv: paths) {
		const Paths &thePaths = kv.second;

		SplitPathRating defaultWorst = {invertersNeededCutoff, 0};
		std::vector<PositionRating> linkResult(result.size(), {defaultWorst, defaultWorst});
		evaluatePositions(thePaths, map, linkResult);

		for (unsigned int posIdx = 0; posIdx < result.size(); ++posIdx) {
			TotalPositionRating &posRating = result[posIdx];
			PositionRating &linkPosRating = linkResult[posIdx];

			for (const SplitPathRating &rating : linkPosRating) {
				if (rating.first >= invertersNeededCutoff)
					continue;

				while (posRating.size() < rating.first + 1) {
					posRating.emplace_back();
				}

				std::array<unsigned int, redstone::maxWireLength + 1> &first = posRating[rating.first];
				assert (rating.second >= 0);
				assert ((unsigned long) rating.second < first.size());
				++first[rating.second];
			}
		}
	}
}


void evaluatePositions(
		const Paths &paths, const SegmentToPositionIndex &map,
		std::vector<PositionRating> &result)
{
	for (const auto &ppath : paths) {
		SplitPathRating worst = {std::numeric_limits<unsigned int>::max(), redstone::maxWireLength};
		std::vector<PositionRating> pathResult(result.size(), {worst, worst});
		evaluatePositions(*ppath, map, pathResult);

		for (unsigned int posIdx = 0; posIdx < result.size(); ++posIdx) {
			PositionRating &best = result[posIdx];
			PositionRating &current = pathResult[posIdx];

			if (current[0] < best[0])
				best = current;
		}
	}
}

SplitPathRating rate(Blocks::index_t length)
{
	constexpr Blocks::index_t mdPlusRepeater = redstone::maxWireLength + 1;
	constexpr unsigned int mdPlusRepeaterShift = 4;
	static_assert((1 << mdPlusRepeaterShift) == mdPlusRepeater, "universe: math error");

	unsigned int repeatersNeeded = length >> mdPlusRepeaterShift;
	Blocks::index_t residue = length - (repeatersNeeded << mdPlusRepeaterShift);
	Blocks::index_t wasted = redstone::maxWireLength - residue;

	return {repeatersNeeded, wasted};
}

PositionRating rate(Blocks::index_t length, Blocks::index_t repeaterPosition)
{
	assert (repeaterPosition < length);

	SplitPathRating rfront = rate(repeaterPosition);
	SplitPathRating rback = rate(length - repeaterPosition - 1);

	if (rback < rfront)
		return {rback, rfront};
	else
		return {rfront, rback};
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

void evaluatePositions(const Path &path, const SegmentToPositionIndex &map, std::vector<PositionRating> &result)
{
	Blocks::index_t absLen = length(path);
	Blocks::index_t freeLen;
	Blocks::index_t repeaterPosition = 0;
	Blocks::index_t skipTo;
	Blocks::index_t stopAt;

	Blocks::index_t posIdx;
	SegmentToPositionIndex::left_map::const_iterator posIter;

	//std::cerr << "Evaluating path " << path << std::endl;

	Segment *segment;
	bool reverse;
	Path::const_iterator curIter = path.begin();

	while (true) {
		skipTo = repeaterPosition;
		freeLen = freeLength(path, skipTo);

		while (freeLen <= redstone::maxWireLength) {
			skipTo += freeLen + 1;

			if (skipTo >= absLen)
				return;

			freeLen = freeLength(path, skipTo);
		}

		stopAt = skipTo + freeLen;

		for (; curIter != path.end(); ++ curIter) {
			segment = curIter -> first;
			reverse = curIter -> second;

			//std::cerr << "  next seg "<< std::endl;

			Blocks::index_t endPosition = repeaterPosition + segment -> distance();

			if ((endPosition <= skipTo) ||
			   ((posIter = map.left.find(segment)) == map.left.end()))
			{
				repeaterPosition = endPosition;
				continue;
			}

			repeaterPosition += segment -> repeaterOffset(reverse);

			if (repeaterPosition >= stopAt) {
				repeaterPosition = stopAt;
				break;
			}

			Blocks::index_t skip = skipTo - repeaterPosition;

			assert (repeaterPosition <= stopAt);

			if (repeaterPosition == stopAt)
				break;

			if (skip < 0)
				skip = 0;

			Blocks::index_t nUnskipped = segment -> repeaterSpace() - skip;

			if (nUnskipped < 0) {
				if (stopAt < endPosition) {
					repeaterPosition = stopAt;
					break;
				}

				repeaterPosition = endPosition;
				continue;
			}

			repeaterPosition += skip;
			int nIterations = std::min(stopAt - repeaterPosition, nUnskipped);

			unsigned int startIdx = posIter -> second + skip;
			unsigned int endIdx = startIdx + nIterations;

			for (posIdx = startIdx; posIdx < endIdx; ++posIdx) {
				//std::cerr << repeaterPosition << ",";
				auto rating = rate(freeLen, repeaterPosition++ - skipTo);
				//std::cerr << posIdx << ": " << segment << ", ";
				//std::cerr << (posIdx - (posIter -> second) + (segment -> repeaterOffset(reverse)));
				//std::cerr << ": " << rating << std::endl;
				result[posIdx] = rating;
			}

			assert (repeaterPosition <= stopAt);

			repeaterPosition += segment -> repeaterOffset(!reverse) - 1;

			if (repeaterPosition >= stopAt)
				repeaterPosition = stopAt;

			if (repeaterPosition == stopAt)
				break;
		}

		assert (repeaterPosition <= absLen);

		if (curIter == path.end() || repeaterPosition == absLen)
			break;

		++repeaterPosition;
	}

	//std::cerr << end << std::endl;
}

Blocks::index_t length(const Path &path)
{
	Blocks::index_t result = 1;

	for (const Current &current : path) {
		const Segment *segment = current.first;
		result += segment -> distance();
	}


	return result;
}

Blocks::index_t freeLength(const Path &path, Blocks::index_t start)
{
	Blocks::index_t curEnd = 0;
	Blocks::index_t curStart = 0;
	Path::const_iterator curIter;
	const Segment *segment;
	bool reverse;

	for (curIter = path.cbegin(); curIter != path.cend(); ++curIter) {
		segment = curIter -> first;
		reverse = curIter -> second;

		curEnd += segment -> distance();

		if (curEnd > start)
			break;

		curStart = curEnd;
	}

	if (curIter == path.cend()) {
		assert (start == curEnd);
		return 1;
	}

	Blocks::index_t curRelPos = start - curStart;
	Blocks::index_t nextRep = segment -> nextRepeater(curRelPos, reverse);

	if (nextRep != -1) {
		assert (nextRep >= curRelPos);
		return nextRep - curRelPos;
	}

	++curIter;
	for (;curIter != path.cend(); ++curIter) {
		segment = curIter -> first;
		reverse = curIter -> second;

		nextRep = segment -> nextRepeater(0, reverse);

		if (nextRep != -1) {
			return curEnd + nextRep - start;
		}

		curEnd += segment -> distance();
	}


	return curEnd - start + 1;
}

bool linkIsBroken(const Link &link, const std::map<Link, Paths> paths)
{
	for (const auto &ppath : paths.at(link)) {
		if (!pathIsBroken(*ppath))
			return false;
	}

	return true;
}

bool pathIsBroken(const Path &path)
{
	return length(path) > redstone::maxWireLength;
}

void eraseWorkingLinks(std::map<Link, Paths> &paths, Links working)
{
	for (const Link &link : working)
		paths.erase(link);

}

void placeRepeater(const RepeaterPlacement &position)
{
	position.first.first -> placeRepeater(position.second, position.first.second);
}

using SegmentContainer = std::vector<std::unique_ptr<UniqueSegment>>;
using GetSegmentContainer = std::function<const SegmentContainer &(const Wire *)>;
using SegmentContainerIterator = boost::transform_iterator<GetSegmentContainer, Connection::iterator>;
using SegmentIterator = CatIterator<SegmentContainerIterator>;
using SegmentIterable = CatGenerator<SegmentContainerIterator>;

static const SegmentContainer &getSegmentContainer(const Wire *w)
{
	return w -> segments();
}

static SegmentContainerIterator segmentContainersBegin(const Connection &connection)
{
	return boost::make_transform_iterator(connection.begin(), getSegmentContainer);
}

static SegmentContainerIterator segmentContainersEnd(const Connection &connection)
{
	return boost::make_transform_iterator(connection.end(), getSegmentContainer);
}

static SegmentIterable allSegments(const Connection &connection)
{
	return SegmentIterable(segmentContainersBegin(connection), segmentContainersEnd(connection));
}

void createSuperSegments(const Connection &connection)
{
	for (const auto &psegment : allSegments(connection)) {
		UniqueSegment *segment = psegment.get();

		if (segment -> hasSuper())
			continue;

		UniqueSegment *before;

		while (true) {
			before = segment -> straightBefore();

			if (!before)
				break;

			segment = before;
		}

		UniqueSegment *next = segment -> straightAfter();

		if (!next)
			continue;

		std::vector<UniqueSegment *> segments = {segment, next};
		segment = next -> straightAfter();

		while (segment) {
			segments.push_back(segment);
			segment = segment -> straightAfter();
		}

		auto superPtr = std::make_shared<SuperSegment>(segments);

		for (const UniqueSegment *segment : segments)
			segment -> setSuper(superPtr);
	}
}

bool fixed(const Path &path)
{
	Blocks::index_t pos = 0;
	Blocks::index_t lastRep = -1;

	for (const Current &current : path) {
		const Segment *segment = current.first;
		bool reverse = current.second;

		Blocks::index_t relLastRep = lastRep - pos;
		Blocks::index_t relNextRep;

		if ((relNextRep = segment -> nextRepeater(0, reverse)) == -1) {
			pos += segment -> distance();
			continue;
		}

		do {
			if (relNextRep - relLastRep > redstone::maxWireLength + 1)
				return false;

			relLastRep = relNextRep;

			if (relLastRep == segment -> distance())
				break;

			relNextRep = segment -> nextRepeater(relLastRep + 1, reverse);
		}
		while (relNextRep != -1);

		lastRep = pos + relLastRep;
		pos += segment -> distance();
	}


	return pos - lastRep <= redstone::maxWireLength;
}

}
