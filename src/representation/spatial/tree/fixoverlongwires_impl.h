#ifndef FIXOVERLONGWIRES_IMPL_H
#define FIXOVERLONGWIRES_IMPL_H

#include "fixoverlongwires.h"
#include "segment.h"

#include "representation/blocks/types.h"

#include "redstone.h"

#include "util/catiterator.h"

#include <boost/iterator/transform_iterator.hpp>

#include <iostream>
#include <set>
#include <list>
#include <unordered_map>
#include <unordered_set>

namespace rhdl::spatial {

class Connector;
class SuperSegment;
class Path;

using Segments = std::vector<const Segment *>;
using GlobalRepeaterPositionToSegment =
		std::map<unsigned int, std::reference_wrapper<Segment>>;

using Paths = std::vector<std::unique_ptr<Path>>;

std::map<Link, Paths> findPaths(const Links &links);
Paths findPaths(const Link &link);

void eraseWorkingLinks(std::map<Link, Paths> &paths);

using RepeaterPlacement = std::pair<Current, blocks::index_t>;

void placeRepeater(const RepeaterPlacement &position);

std::unordered_set<Segment *> identifyCurrents(const std::map<Link, Paths> &paths);

unsigned int makePositionMap(
		std::unordered_set<Segment *> segments,
		GlobalRepeaterPositionToSegment &result);

using BestPlacementResult = std::pair<RepeaterPlacement, Links>;

RepeaterPlacement findBestPlacement(const std::map<Link, Paths> &paths,
		const GlobalRepeaterPositionToSegment &map,
		unsigned int nPositions);

using TotalPositionRating = std::vector<std::array<unsigned int, redstone::maxWireLength + 1>>;

struct TotalPositionResult {
	//~TotalPositionResult();

	TotalPositionRating rating_;
	std::vector<Link> linksFixed_;
};

RepeaterPlacement findBestPlacement(
		const GlobalRepeaterPositionToSegment &map,
		const std::vector<TotalPositionRating> &evaluatedPositions);

RepeaterPlacement getPlacementFromIdx(
		unsigned int positionIdx,
		const GlobalRepeaterPositionToSegment &map);

void evaluatePositions(
		const std::map<Link, Paths> &paths,
		std::vector<TotalPositionRating> &result);

using SplitPathRating = std::pair<unsigned int, blocks::index_t>;
using PositionRating = std::array<SplitPathRating, 2>;

struct PositionResult {
	PositionRating rating_;
	std::vector<Link> linksFixed_;
};

void evaluatePositions(
		const Paths &paths, std::vector<PositionRating> &result);

bool fixed(const Path &path);

PositionRating rate(blocks::index_t length, blocks::index_t repeaterPosition);
SplitPathRating rate(blocks::index_t length);

std::ostream &operator<<(std::ostream &os, const Path &path);

}

#endif // FIXOVERLONGWIRES_IMPL_H
