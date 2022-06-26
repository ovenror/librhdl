#include "fixoverlongwires_impl.h"

#include "connection.h"
#include "connector.h"
#include "layer.h"
#include "node.h"
#include "supersegment.h"
#include "uniquesegment.h"
#include "wire.h"
#include "path.h"
#include "pathbuilder.h"

#include "representation/blocks/types.h"

#include "redstone.h"

#include <deque>
#include <functional>
#include <memory>
#include <limits>
#include <unordered_set>

namespace rhdl::spatial {

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

	auto all_segments = identifyCurrents(paths);

	GlobalRepeaterPositionToSegment segmentToPositionIdx;
	unsigned int nPositions = makePositionMap(all_segments, segmentToPositionIdx);

	eraseWorkingLinks(paths);

	if (paths.empty())
		return FixConnectionResult::UNCHANGED;

	RepeaterPlacement bestPosition;

	while (!paths.empty()) {
		bestPosition = findBestPlacement(paths, segmentToPositionIdx, nPositions);

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

	Paths paths;

	if (sStart == sEnd) {
		paths.emplace_back(std::make_unique<Path>(Path({})));
		return paths;
	}

	std::vector<PathBuilder> pathBuilders = {PathBuilder(*sStart)};
	std::vector<PathBuilder> nextPathBuilders;

	for (unsigned int len = 1; !pathBuilders.empty(); ++len) {
		for (const auto &pathBuilder : pathBuilders) {
			assert (pathBuilder.size() == len - 1);

			//std::cerr << "extending..." << std::endl;
			//std::cerr << pathBuilder << std::endl;

			for (const Current &current : pathBuilder.head().superConnected()) {
				const Segment *segment = current.first;

				//std::cerr << "  trying..." << std::endl;
				//std::cerr << "  " << current << std::endl;

				if (pathBuilder.contains(*segment))
					continue;

				//std::cerr << "  new..." << std::endl;

				auto next = pathBuilder + current;

				if (&next.head() == sEnd) {
					//std::cerr << "   !!!FOUND PATH!!!" << std::endl;
					paths.emplace_back(next.build());
				}
				else {
					//std::cerr << "   ready for next round" << std::endl;
					nextPathBuilders.emplace_back(std::move(next));
				}
			}
		}

		pathBuilders = std::move(nextPathBuilders);
		nextPathBuilders = {};
	}

	assert (!paths.empty());

	return paths;
}

std::unordered_set<Segment *> identifyCurrents(
		const std::map<Link, Paths> &paths)
{
	std::unordered_set<Segment *> segments;

	for (const auto &kv : paths) {
		for (const auto &ppath : kv.second) {
			for (const PathElement &element : *ppath) {
				auto &segment = element.segment();

				segment.useDirection(element.reverse());
				segments.emplace(&segment);
			}
		}
	}

	return segments;
}

std::ostream &operator<<(std::ostream &os, const Path &path)
{
	os << "PATH" << std::endl;

	for (const auto &e: path) {
		os << "  " << e << std::endl;
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
		std::unordered_set<Segment *>  segments,
		GlobalRepeaterPositionToSegment &result)
{
	unsigned int idx = 0;

	for (auto *segment : segments) {
		if (!segment -> isOneway())
			continue;

		blocks::index_t space = segment -> repeaterSpace();

		if (space <= 0)
			continue;

		//std::cerr << "adding " << *segment << " to map: " << idx;

		idx += space;
		result.emplace(idx, std::ref(*segment));
		segment -> setGlobalRepeaterPositionsEndIdx(idx);
		//std::cerr << "--" << idx-1 << std::endl;
	}

	return idx;
}

RepeaterPlacement findBestPlacement(const std::map<Link, Paths> &paths,
		const GlobalRepeaterPositionToSegment &map,
		unsigned int nPositions)
{    
	std::vector<TotalPositionRating> evaluatedPositions(nPositions);

	evaluatePositions(paths, evaluatedPositions);
	return findBestPlacement(map, evaluatedPositions);
}

RepeaterPlacement findBestPlacement(
		const GlobalRepeaterPositionToSegment &map,
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

		RepeaterPlacement placement = getPlacementFromIdx(posIdx, map);

		bestPosition = placement;
		bestRating = &posRating;
	}

	return bestPosition;
}

RepeaterPlacement getPlacementFromIdx(
		unsigned int positionIdx,
		const GlobalRepeaterPositionToSegment &map)
{
	auto iter = map.upper_bound(positionIdx);
	assert (iter != map.end());

	Segment &segment = iter -> second;
	bool reverse = segment.getOnewayReverse();
	auto offset = segment.globalRepeaterPositionIdxToLocalOffset(positionIdx);
	return {{&segment, reverse}, offset};
}

void evaluatePositions(
		const std::map<Link, Paths> &paths,
		std::vector<TotalPositionRating> &result)
{
	constexpr unsigned int invertersNeededCutoff = 10;

	for (const auto &kv: paths) {
		const Paths &thePaths = kv.second;

		SplitPathRating defaultWorst = {invertersNeededCutoff, 0};
		std::vector<PositionRating> linkResult(result.size(), {defaultWorst, defaultWorst});
		evaluatePositions(thePaths, linkResult);

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
		const Paths &paths,	std::vector<PositionRating> &result)
{
	for (const auto &ppath : paths) {
		SplitPathRating worst = {std::numeric_limits<unsigned int>::max(), redstone::maxWireLength};
		std::vector<PositionRating> pathResult(result.size(), {worst, worst});
		ppath -> evaluatePositions(pathResult);

		for (unsigned int posIdx = 0; posIdx < result.size(); ++posIdx) {
			PositionRating &best = result[posIdx];
			PositionRating &current = pathResult[posIdx];

			if (current[0] < best[0])
				best = current;
		}
	}
}

SplitPathRating rate(blocks::index_t length)
{
	constexpr blocks::index_t mdPlusRepeater = redstone::maxWireLength + 1;
	constexpr unsigned int mdPlusRepeaterShift = 4;
	static_assert((1 << mdPlusRepeaterShift) == mdPlusRepeater, "universe: math error");

	unsigned int repeatersNeeded = length >> mdPlusRepeaterShift;
	blocks::index_t residue = length - (repeatersNeeded << mdPlusRepeaterShift);
	blocks::index_t wasted = redstone::maxWireLength - residue;

	return {repeatersNeeded, wasted};
}

PositionRating rate(blocks::index_t length, blocks::index_t repeaterPosition)
{
	assert (repeaterPosition < length);

	SplitPathRating rfront = rate(repeaterPosition);
	SplitPathRating rback = rate(length - repeaterPosition - 1);

	if (rback < rfront)
		return {rback, rfront};
	else
		return {rfront, rback};
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
	blocks::index_t pos = 0;
	blocks::index_t lastRep = -1;

	for (const Current &current : path) {
		const Segment *segment = current.first;
		bool reverse = current.second;

		blocks::index_t relLastRep = lastRep - pos;
		blocks::index_t relNextRep;

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
