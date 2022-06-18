#ifndef FIXOVERLONGWIRES_IMPL_H
#define FIXOVERLONGWIRES_IMPL_H

#include "representation/blocks/blocks.h"

#include "redstone.h"

#include "util/catiterator.h"

#include <boost/bimap.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include <iostream>
#include <set>
#include <list>
#include "../tree/connection.h"
#include "../tree/fixoverlongwires.h"
#include "../tree/segment.h"

namespace rhdl::spatial {

class Connector;
class SuperSegment;

using Segments = std::vector<const Segment *>;
using SegmentToPositionIndex = boost::bimap<Segment *, unsigned int>;

using Path = std::vector<Current>;
using Paths = std::vector<std::unique_ptr<Path>>;

std::map<Link, Paths> findPaths(const Links &links);
Paths findPaths(const Link &link);

void eraseWorkingLinks(std::map<Link, Paths> &paths, Links working);

using RepeaterPlacement = std::pair<Current, blocks::Blocks::index_t>;

void placeRepeater(const RepeaterPlacement &position);

bool linkIsBroken(const Link &link, const std::map<Link, Paths> paths);
bool pathIsBroken(const Path &path);

const std::map<Segment *, bool> identifyEligibleCurrents(const std::map<Link, Paths> &paths);

unsigned int makePositionMap(const std::map<Segment *, bool> &eligible,
		SegmentToPositionIndex &result);

using BestPlacementResult = std::pair<RepeaterPlacement, Links>;

RepeaterPlacement findBestPlacement(const std::map<Link, Paths> &paths,
		const std::map<Segment *, bool> &currents,
		const SegmentToPositionIndex &map,
		unsigned int nPositions);

using TotalPositionRating = std::vector<std::array<unsigned int, redstone::maxWireLength + 1>>;

struct TotalPositionResult {
	//~TotalPositionResult();

	TotalPositionRating rating_;
	std::vector<Link> linksFixed_;
};

RepeaterPlacement findBestPlacement(const std::map<Segment *, bool> &currents,
		const SegmentToPositionIndex &map,
		const std::vector<TotalPositionRating> &evaluatedPositions);

RepeaterPlacement getPlacementFromIdx(unsigned int positionIdx,
		const std::map<Segment *, bool> &currents,
		const SegmentToPositionIndex &map);

void evaluatePositions(const std::map<Link, Paths> &paths, const SegmentToPositionIndex &map,
		std::vector<TotalPositionRating> &result);

using SplitPathRating = std::pair<unsigned int, blocks::Blocks::index_t>;
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

blocks::Blocks::index_t length(const Path &path);
blocks::Blocks::index_t freeLength(
		const Path &path, blocks::Blocks::index_t start);

bool fixed(const Path &path);

PositionRating rate(blocks::Blocks::index_t length, blocks::Blocks::index_t repeaterPosition);
SplitPathRating rate(blocks::Blocks::index_t length);

std::ostream &operator<<(std::ostream &os, const Path &path);

}

#endif // FIXOVERLONGWIRES_IMPL_H
