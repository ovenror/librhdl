#ifndef FIXOVERLONGWIRES_IMPL_H
#define FIXOVERLONGWIRES_IMPL_H

#include "fixoverlongwires.h"
#include "connection.h"
#include "segment.h"

#include "representation/blocks/blocks.h"

#include "redstone.h"

#include "util/catiterator.h"

#include <boost/bimap.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include <iostream>
#include <set>
#include <list>

namespace rhdl::TM {

class Connector;
class SuperSegment;

using Segments = std::vector<const Segment *>;
using SegmentToPositionIndex = boost::bimap<const Segment *, unsigned int>;

using Path = std::vector<Current>;
using Paths = std::vector<std::unique_ptr<Path>>;

bool fixBrokenConnection(const TM::Connection &n, const WorkingAndBrokenLinks &brokenLinks, Blocks &b);

void createSuperSegments(const Connection &connection);

std::map<Link, Paths> findPaths(const WorkingAndBrokenLinks &links);
Paths findPaths(const Link &link);

void eraseWorkingLinks(std::map<Link, Paths> &paths, Links working);

using RepeaterPlacement = std::pair<Current, Blocks::index_t>;

void placeRepeater(const RepeaterPlacement &position, Blocks &b);

bool linkIsBroken(const Link &link, const std::map<Link, Paths> paths);
bool pathIsBroken(const Path &path);

const std::map<const Segment *, bool> identifyEligibleCurrents(const std::map<Link, Paths> &paths);

unsigned int makePositionMap(const std::map<const Segment *, bool> &eligible,
		SegmentToPositionIndex &result);

using BestPlacementResult = std::pair<RepeaterPlacement, Links>;

RepeaterPlacement findBestPlacement(const std::map<Link, Paths> &paths,
		const std::map<const Segment *, bool> &currents,
		const SegmentToPositionIndex &map,
		unsigned int nPositions);

using TotalPositionRating = std::vector<std::array<unsigned int, redstone::maxWireLength + 1>>;

struct TotalPositionResult {
	//~TotalPositionResult();

	TotalPositionRating rating_;
	std::vector<Link> linksFixed_;
};

RepeaterPlacement findBestPlacement(const std::map<const Segment *, bool> &currents,
		const SegmentToPositionIndex &map,
		const std::vector<TotalPositionRating> &evaluatedPositions);

RepeaterPlacement getPlacementFromIdx(unsigned int positionIdx,
		const std::map<const Segment *, bool> &currents,
		const SegmentToPositionIndex &map);

void evaluatePositions(const std::map<Link, Paths> &paths, const SegmentToPositionIndex &map,
		std::vector<TotalPositionRating> &result);

using SplitPathRating = std::pair<unsigned int, Blocks::index_t>;
using PositionRating = std::array<SplitPathRating, 2>;

struct PositionResult {
	PositionRating rating_;
	std::vector<Link> linksFixed_;
};

void evaluatePositions(const Paths &paths, const SegmentToPositionIndex &map,
		std::vector<PositionRating> &result);

void evaluatePositions(
		const Path &path, const SegmentToPositionIndex &map,
		std::vector<PositionRating> &result);

Blocks::index_t length(const Path &path);
Blocks::index_t freeLength(
		const Path &path, Blocks::index_t start);

bool fixed(const Path &path);

PositionRating rate(Blocks::index_t length, Blocks::index_t repeaterPosition);
SplitPathRating rate(Blocks::index_t length);

std::ostream &operator<<(std::ostream &os, const Path &path);

}

#endif // FIXOVERLONGWIRES_IMPL_H
