#ifndef WIREBASE_H
#define WIREBASE_H

#include "representation/blocks/types.h"

#include <set>
#include <functional>
#include "../tree/crosser.h"
#include "../tree/element.h"

namespace rhdl {
namespace spatial {

class Container;
class Wires;
class Crosser;
class Node;
class NodeGroup;
class Layer;
class Connection;

class AtomicSegment;
class Connector;

class Wire : public Element
{
public:
	struct Less;
	using Range = std::pair<blocks::index_t, blocks::index_t>;

	Wire(Container &owner, bool anchor = false, unsigned int index = 0);
	Wire(Wire &&dying, Container &newOwner);

	~Wire();

	Node *getNode() const;
	NodeGroup *getNodeGroup() const;
	Layer* getLayer() const;

	virtual Wires *collection() const = 0;
	virtual bool vertical() const = 0;

	virtual Crosser &asCrosser() = 0;
	virtual const Crosser &asCrosser() const = 0;

	void connect(const std::shared_ptr<Connection> &to);
	void connect(const Wire &w);
	void disconnect();

	bool isConnected(const Connection &c) const;
	bool isConnected(const Wire &w) const;
	bool isConnected(const Wires &c) const;

	bool isInputOf(const Node *n) const;
	bool isOutputOf(const Node *n) const;

	const Node *getInverter() const;

	bool isInverterInput() const;
	bool isInverterOutput() const;

	bool isInputOfInverter(const Node *n) const;
	bool isOutputOfInverter(const Node *n) const;

	virtual const std::set<Crosser *> &getCrossers() const = 0;

	bool mayCross(const Crosser &crosser, blocks::index_t at) const;

	bool isCrosser(const Wire &w) const;

	bool isCrossConnected() const;
	bool isCrossConnected(const Wire &w) const;

	bool isImmediatelyConnected(const Wire &w) const;
	bool occupiesSameXY(const Wire &w) const;
	std::vector<Wire *> wiresConnectedAt(blocks::index_t cpos) const;

	bool isCrossedBy(const Wire &w) const;
	bool crosses(const Wire &w) const;
	bool isInCrossingRangeOf(const Wire &w) const;

	void computeAbsolutePosition();
	void computeAbsolutePosition(blocks::index_t offset);
	void setPosition(blocks::index_t position);

	void tryInsertCrosser_internal(Wire &crosser);
	void replaceCrosser_internal(Wire &newCrosser);
	virtual bool mayBeCrossed_internal(const Wire &by, blocks::index_t at) const;

	virtual void dropHorizontalCollected() {}
	bool hasDisjunctExtentsWith(const Wire &other) const;
	bool hasOverlappingExtentsWith(const Wire &other) const;

	std::vector<Range> freeRanges(blocks::index_t minSize) const;

	void computeExtents();
	void tryBecomeAnchor();
	void toBlocks(blocks::Cuboid b) const;

	void createSegments();

	bool hasSegments() const;
	const std::vector<std::unique_ptr<AtomicSegment>> &segments() const {return atomicSegments_;}

	const Connector &front() const;
	const Connector &back() const;

	Container &owner_;
	bool anchor_;
	std::shared_ptr<Connection> connection_;
	bool has_extents_;
	blocks::index_t relativePosition_, start_, end_;

	struct Less {
		using is_transparent = void;

		bool operator()(const Wire* const & lhs, const Wire* const & rhs) const
		{
			return lhs -> position() < rhs -> position();
		}

		bool operator()(const Wire* const & lhs, blocks::index_t rhs) const
		{
			return lhs -> position() < rhs;
		}

		bool operator()(blocks::index_t lhs, const Wire* const & rhs) const
		{
			return lhs < rhs -> position();
		}
	};

	using SortedCrossers = std::set<Wire *, Less>;

	const SortedCrossers &getSortedCrossers() const {return sorted_crossers_;}

	static void blocks(int height_offset, blocks::Wall line_segment, blocks::index_t position, blocks::index_t length);
	blocks::Wall segment(blocks::Cuboid &blocks) const;

	void addRepeater(blocks::index_t position, bool backwards);
	void placeRepeaters(blocks::Cuboid) const;

protected:
	std::vector<Wire *> wiresCrossConnectedAt(blocks::index_t pos) const;

	static blocks::Line line(blocks::Wall segment, blocks::index_t height, blocks::index_t position, blocks::index_t length);

	void blocks_isolated(blocks::Wall line_segment, blocks::index_t position, blocks::index_t length) const;
	void blocks_connected(blocks::Wall line_segment, blocks::index_t position, blocks::index_t length) const;

	SortedCrossers sorted_crossers_;

public:
	Wire &operator=(const Wire &) = delete;
	Wire &operator=(Wire &&) = delete;
	Wire (const Wire &) = delete;

	friend std::ostream &operator<<(std::ostream &os, const Wire &w);

private:
	struct Repeater {
		blocks::index_t position;
		bool backwards;
	};

	struct ConnectorLess {
		using is_transparent = void;

		ConnectorLess(const Wire &This);

		bool operator()(
				const std::shared_ptr<Connector> &lhs,
				const std::shared_ptr<Connector> &rhs
				) const;

		bool operator()(
				const std::shared_ptr<Connector> &lhs,
				blocks::index_t rhs
				) const;

		bool operator()(
				blocks::index_t lhs,
				const std::shared_ptr<Connector> &rhs
				) const;

	private:
		const Wire &this_;
	};

	AtomicSegment &addSegment(blocks::index_t start, blocks::index_t end,
			Connector *startSeg, Connector *endSeg);

	Connector *addConnector(blocks::index_t pos);

	Connector &getConnectorAt(blocks::index_t pos) const;
	const Connector &onlyConnector() const;

	void placeRepeater(const Repeater &, blocks::Cuboid) const;

	std::vector<std::unique_ptr<AtomicSegment>> atomicSegments_;
	std::set<std::shared_ptr<Connector>, ConnectorLess> connectors_;
	std::set<std::unique_ptr<Repeater>> repeaters_;
};

std::ostream &operator<<(std::ostream &os, const Wire &w);

}
}

#endif // WIREBASE_H
