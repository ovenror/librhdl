#ifndef DIVISIVEWIRES_H
#define DIVISIVEWIRES_H

#include <boost/multi_array.hpp>

#include <array>
#include <set>
#include <list>
#include <forward_list>
#include "../tree/wires.h"

namespace rhdl {
namespace spatial {

class DivisiveWires : public Wires
{
public:
	DivisiveWires(Container &owner, bool vertical = false, bool anchors = false);

	std::unique_ptr<SingleWire> release(const Wire &wire) override;

	void tryInsertCrosser(Wire &crosser) override;
	void replaceCrosser(Wire &newCrosser) override;
	bool mayBeCrossed(const Wire &wire, blocks::Blocks::index_t at) const override;
	bool mayBeCrossed(const CollectedWire &liaison, const Wire &newCrossingWire, blocks::Blocks::index_t at) const;

	void computePositions() override;
	bool dropCollision(const CollectedWire &dropped, blocks::Blocks::index_t at, const CollectedWire &other) const override;

private:
	struct PrimaryComparator;
	using SortedCrossers = Wire::SortedCrossers;

	CollectedWire &add(std::unique_ptr<CollectedWire> &&newWire) override;

	std::forward_list<CollectedWire *> ascendingOrder() const;
	bool checkStrictPartialOrdering() const;

	bool less(const CollectedWire &lesser, const CollectedWire &greater) const;
	bool compare(const CollectedWire &lesser, const CollectedWire &greater, bool reverse = false) const;
	void extendRelation(CollectedWire &liaison, const Wire &crosser);
	void makeLess(CollectedWire &lesser, CollectedWire &greater);

	bool connectedBelow(const CollectedWire &wire, blocks::Blocks::index_t pos) const;
	bool connectedAbove(const CollectedWire &wire, blocks::Blocks::index_t pos) const;
	bool connected(const Wire &crosser, bool below) const;
	bool connected(const CollectedWire &wire, blocks::Blocks::index_t pos, bool below) const;

	CollectedWire *connected(blocks::Blocks::index_t pos, bool below) const;

	SortedCrossers &crossers(const Wire &wire);
	const SortedCrossers &crossers(const Wire &wire) const;
	SortedCrossers &crossers(bool below);
	const SortedCrossers &crossers(bool below) const;

	const Wire *crosser(blocks::Blocks::index_t pos, bool below) const;

	bool fromBelow(const Wire &wire) const;
	bool fromAbove(const Wire &wire) const {return !fromBelow(wire);}

	void orderToStream(std::ostream &os, const std::forward_list<CollectedWire *> &order) const;
	void debugMatrix() const;

	struct SecondaryComparator {
		SecondaryComparator(const DivisiveWires& that);
		bool operator()(const CollectedWire *lhs, const CollectedWire *rhs) const;
		const DivisiveWires &that_;
	};

	static constexpr unsigned int maxWires = 64;

	SortedCrossers crossersFromAbove_;
	SortedCrossers crossersFromBelow_;
	std::vector<std::forward_list<CollectedWire *>> predecessors_;
	std::vector<unsigned int> numSuccessors_;
	boost::multi_array<bool, 2> less_;
};

}
}

#endif // DIVISIVEWIRES_H
