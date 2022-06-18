#include "../tree/divisivewires.h"

#include "util/iterable.h"

#include <utility>
#include <stack>
#include <iomanip>
#include "../tree/layer.h"
#include "../tree/singlewire.h"

namespace rhdl {
namespace spatial {

using blocks::Blocks;

DivisiveWires::DivisiveWires(Container &owner, bool vertical, bool anchors) :
	Wires(owner, vertical, anchors)
{}

CollectedWire &DivisiveWires::add(std::unique_ptr<CollectedWire> &&newWire)
{
	assert(nextIndex_ >= collection_.size());
	auto currentIndex = nextIndex_ - 1;
	auto &result = Wires::add(std::move(newWire));
	assert(result.index() == currentIndex);
	assert(predecessors_.size() == currentIndex);
	assert(numSuccessors_.size() == currentIndex);

	predecessors_.emplace_back();
	numSuccessors_.push_back(0);
	less_.resize(boost::extents[nextIndex_][nextIndex_]);

	return result;
}

std::unique_ptr<SingleWire> DivisiveWires::release(const Wire &wire)
{
	for (Wire *crosser : wire.getSortedCrossers()) {
		SortedCrossers &dcrossers = crossers(*crosser);
		assert (dcrossers.erase(crosser));
	}

	std::unique_ptr<SingleWire> result = Wires::release(wire);

	assert (checkStrictPartialOrdering());
	return result;
}

void DivisiveWires::tryInsertCrosser(Wire &crosser)
{
	if (collection_.empty())
		return;

	CollectedWire &liaison = find(*crosser.connection_);
	assert (mayBeCrossed(liaison, crosser, crosser.position()));
	liaison.tryInsertCrosser_internal(crosser);
	extendRelation(liaison, crosser);
	assert (checkStrictPartialOrdering());

	SortedCrossers &dcrossers = crossers(crosser);
	auto result = dcrossers.insert(&crosser);
	assert (result.second);
}

void DivisiveWires::replaceCrosser(Wire &newCrosser)
{
	SortedCrossers &dcrossers = crossers(newCrosser);
	assert (dcrossers.erase(&newCrosser));
	dcrossers.insert(&newCrosser);

	for (auto &pwire : collection()) {
		const SortedCrossers &crossers = pwire -> getSortedCrossers();

		if (crossers.find(&newCrosser) != crossers.end())
			pwire -> replaceCrosser_internal(newCrosser);
	}
}

bool DivisiveWires::mayBeCrossed(const Wire &newCrossingWire, Blocks::index_t at) const
{
	if (collection_.empty())
		return true;

	CollectedWire &connected = find(*newCrossingWire.connection_);
	return mayBeCrossed(connected, newCrossingWire, at);
}

bool DivisiveWires::mayBeCrossed(const CollectedWire &liaison, const Wire &newCrossingWire, Blocks::index_t at) const
{
	assert(!liaison.anchor_);
	assert(!newCrossingWire.anchor_);
	assert(checkStrictPartialOrdering());

	const SortedCrossers &dcrossers = crossers(newCrossingWire);
	if (dcrossers.find(at) != dcrossers.end())
		return false;

	bool newFromAbove = fromAbove(newCrossingWire);

	const CollectedWire::SortedCrossers &connCrossers = liaison.getSortedCrossers();
	CollectedWire::SortedCrossers::iterator existingCrosser = connCrossers.find(at);
	if (existingCrosser != connCrossers.end()) {
		assert (fromAbove(**existingCrosser) != newFromAbove);
		return true;
	}

	const CollectedWire *counterpart = connected(at, newFromAbove);

	if (!counterpart)
		return true;

	return !compare(*counterpart, liaison, newFromAbove);
}

void DivisiveWires::debugMatrix() const
{
	std::cerr << "    ";
	for (const auto &w2 : collection_)
		std::cerr << std::setw(2) << w2 -> index();

	std::cerr << std::endl;
	for (const auto &w1 : collection_) {
		std::cerr << std::setw(2) << w1 -> index() << ": ";
		for (const auto &w2 : collection_) {
			std::cerr << " ";

			if (less(*w1, *w2) && less(*w2, *w1))
				std::cerr << "!";
			else if (less(*w1, *w2))
				std::cerr << "<";
			else if (less(*w2, *w1))
				std::cerr << ">";
			else
				std::cerr << " ";
		}
		std::cerr << std::endl;
	}
}

std::forward_list<CollectedWire *> DivisiveWires::ascendingOrder() const
{
	assert (checkStrictPartialOrdering());

	std::forward_list<CollectedWire *> result;
	std::set<CollectedWire *, SecondaryComparator> primaryEquals(SecondaryComparator(*this));

	auto remainingSucc = numSuccessors_;

	for (const auto &wire : collection_)
		if (remainingSucc[wire -> index()] == 0)
			primaryEquals.insert(wire.get());

	while (!primaryEquals.empty()) {
		auto inext = primaryEquals.begin();
		CollectedWire &next = **inext;
		primaryEquals.erase(inext);
		result.push_front(&next);

		for (auto *pred : predecessors_[next.index()]) {
			auto predIdx = pred -> index();
			if (--remainingSucc[predIdx] == 0)
				primaryEquals.insert(pred);
		}
	}

	//orderToStream(std::cerr, result);

	return result;
}

bool DivisiveWires::connectedBelow(const CollectedWire &wire, Blocks::index_t pos) const
{
	return connected(wire, pos, true);
}

bool DivisiveWires::connectedAbove(const CollectedWire &wire, Blocks::index_t pos) const
{
	return connected(wire, pos, false);
}

bool DivisiveWires::connected(const Wire &crosser, bool below) const
{
	return connected(crosser.position(), below);
}

bool DivisiveWires::connected(const CollectedWire &wire, Blocks::index_t pos, bool below) const
{
	assert (wire.collection() == this);
	auto theCrosser = crosser(pos, below);
	return theCrosser && wire.isConnected(*theCrosser);
}

const Wire *DivisiveWires::crosser(Blocks::index_t pos, bool below) const
{
	const auto &theCrossers = crossers(below);
	auto crosser = theCrossers.find(pos);

	if (crosser != theCrossers.end())
		return *crosser;

	return nullptr;
}

CollectedWire *DivisiveWires::connected(Blocks::index_t pos, bool below) const
{
	auto theCrosser = crosser(pos, below);

	if (!theCrosser)
		return nullptr;

	for (const auto &wire : collection_)
		if (wire -> isConnected(*theCrosser))
			return wire.get();

	return nullptr;
}

void DivisiveWires::computePositions()
{
	Blocks::index_t relpos = 0;
	auto order = ascendingOrder();

	for (Wire *wire : order) {
		wire -> relativePosition_ = relpos;
		relpos += 2;
	}

	for (Wire *wire : order)
		wire -> dropHorizontalCollected();
}

bool DivisiveWires::dropCollision(const CollectedWire &dropped, Blocks::index_t at, const CollectedWire &other) const
{
	assert (!dropped.anchor_);
	assert (!other.anchor_);
	assert (at < other.relativePosition_ || !less(dropped, other));

	if (at <= other.relativePosition_ && less(other, dropped))
		return true;

	return Wires::dropCollision(dropped, at, other);
}

bool DivisiveWires::checkStrictPartialOrdering() const
{
	//debugMatrix();

	for (unsigned i=0; i < nextIndex_; ++i)
		for (unsigned  j=0; j < i; ++j)
			if (less_[i][j] && less_[j][i])
				return false;

	return true;
}

void DivisiveWires::makeLess(CollectedWire &lesser, CollectedWire &greater)
{
	assert (&lesser != &greater);

	auto le = lesser.index();
	auto gr = greater.index();

	//std::cerr << le << " < " << gr << std::endl;

	assert (le != gr);

	predecessors_[gr].push_front(&lesser);
	++numSuccessors_[le];
	less_[le][gr] = true;

	for (unsigned i=0; i < nextIndex_; ++i) {
		less_[i][gr] |= less_[i][le];
		less_[le][i] |= less_[gr][i];
	}

	for (unsigned i=0; i < nextIndex_; ++i)
		for (unsigned j=0; j < nextIndex_; ++j)
			less_[i][j] |= less_[i][le] && less_[gr][j];

#if 0
	for (const auto &wire : collection_) {
		auto idx = wire -> index();
		if (numSuccessors_[idx] > 0)
			continue;

		std::cerr << "start wirh " << idx << std::endl;;
		auto predecessors = predecessors_[idx];

		while (!predecessors.empty()) {
			decltype(predecessors) nextPreds;

			for (const auto *pred : predecessors) {
				std::cerr << "test " << pred->index() << " < " << wire->index() << std::endl;
				assert(less(*pred, *wire));
				const auto &predpreds = predecessors_[pred -> index()];
				std::copy(predpreds.begin(), predpreds.end(), std::front_inserter(nextPreds));
			}

			predecessors = nextPreds;
		}
	}
#endif
}

bool DivisiveWires::less(const CollectedWire &lesser, const CollectedWire &greater) const
{
	return less_[lesser.index()][greater.index()];
}

bool DivisiveWires::compare(const CollectedWire &lesser, const CollectedWire &greater, bool reverse) const
{
	if (reverse)
		return less(greater, lesser);
	else
		return less(lesser, greater);
}

void DivisiveWires::extendRelation(CollectedWire &liaison, const Wire &crosser)
{
	auto *counterpart = connected(crosser.position(), !fromBelow(crosser));

	if (!counterpart || counterpart == &liaison)
		return;

	if (fromBelow(crosser))
		makeLess(liaison, *counterpart);
	else
		makeLess(*counterpart, liaison);
}

DivisiveWires::SortedCrossers &DivisiveWires::crossers(const Wire &wire)
{
	return crossers(fromBelow(wire));
}

const DivisiveWires::SortedCrossers &DivisiveWires::crossers(const Wire &wire) const
{
	return crossers(fromBelow(wire));
}

DivisiveWires::SortedCrossers &DivisiveWires::crossers(bool below)
{
	return below ? crossersFromBelow_ : crossersFromAbove_;
}

const DivisiveWires::SortedCrossers &DivisiveWires::crossers(bool below) const
{
	return below ? crossersFromBelow_ : crossersFromAbove_;
}

bool DivisiveWires::fromBelow(const Wire &wire) const
{
	const Layer *ownLayer = owner_.getLayer();
	const Layer *wLayer = wire.getLayer();

	if (wLayer == ownLayer)
		return true;

	assert (wLayer);
	const Container &wContainer = wLayer -> asContainer();
	assert ((!ownLayer && wContainer.index() == 0) ||
			wContainer.index() == ownLayer -> asContainer().index() + 1);

	return false;
}

void DivisiveWires::orderToStream(std::ostream &os, const std::forward_list<CollectedWire *> &order) const
{
	for (const auto *wire : order) {
		//if (wire -> anchor_)
		//    continue;

		os << std::setw(2) << wire -> index();

		auto sortedCrossers = wire -> getSortedCrossers();
		auto start = (*sortedCrossers.begin()) -> position();
		auto end = (*sortedCrossers.rbegin()) -> position();

		for (Blocks::index_t pos = 0; pos < start; ++pos)
			os << " ";

		for (auto pos = start; pos <= end; ++pos) {
			Wire::SortedCrossers::iterator crosser;

			if ((crosser = sortedCrossers.find(pos)) == sortedCrossers.end()) {
				os << "-";
				continue;
			}

			bool cAbove = connectedAbove(*wire, pos);
			bool cBelow = connectedBelow(*wire, pos);

			if (!cAbove && !cBelow) {
				os << "-";
				continue;
			}

			if (cAbove && cBelow) {
				os << "*";
				continue;
			}

			if (cAbove)
				os << "^";
			else
				os << "v";
		}

		os << std::endl;
	}

	os << std::endl;
}

DivisiveWires::SecondaryComparator::SecondaryComparator(const DivisiveWires &that)
	: that_(that)
{}

bool DivisiveWires::SecondaryComparator::operator()(const CollectedWire *lhs, const CollectedWire *rhs) const
{
	if (that_.numIsolatedUpperInputCrossings(*lhs) <
		that_.numIsolatedUpperInputCrossings(*rhs))
		return true;

	if (that_.numIsolatedUpperInputCrossings(*rhs) <
		that_.numIsolatedUpperInputCrossings(*lhs))
		return false;

	return lhs -> index() > rhs -> index();
}

}
}
