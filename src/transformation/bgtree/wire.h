#ifndef WIREBASE_H
#define WIREBASE_H

#include "representation/blocks/blocks.h"

#include "crosser.h"
#include "element.h"

#include <set>
#include <functional>

namespace rhdl {
namespace TM {

class Container;
class Wires;
class Crosser;
class Node;
class NodeGroup;
class Layer;
class Connection;

class UniqueSegment;
class Connector;

class Wire : public Element
{
public:
	struct Less;
	using Range = std::pair<Blocks::index_t, Blocks::index_t>;

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

	bool mayCross(const Crosser &crosser, Blocks::index_t at) const;

	bool isCrosser(const Wire &w) const;

	bool isCrossConnected() const;
	bool isCrossConnected(const Wire &w) const;

	bool isImmediatelyConnected(const Wire &w) const;
	bool occupiesSameXY(const Wire &w) const;
	std::vector<Wire *> wiresConnectedAt(Blocks::index_t cpos) const;

	bool isCrossedBy(const Wire &w) const;
	bool crosses(const Wire &w) const;
	bool isInCrossingRangeOf(const Wire &w) const;

	void computeAbsolutePosition();
	void computeAbsolutePosition(Blocks::index_t offset);
	void setPosition(Blocks::index_t position);

	void tryInsertCrosser_internal(Wire &crosser);
	void replaceCrosser_internal(Wire &newCrosser);
	virtual bool mayBeCrossed_internal(const Wire &by, Blocks::index_t at) const;

	virtual void dropHorizontalCollected() {}
	bool hasDisjunctExtentsWith(const Wire &other) const;
	bool hasOverlappingExtentsWith(const Wire &other) const;

	std::vector<Range> freeRanges(Blocks::index_t minSize) const;

	void computeExtents();
	void tryBecomeAnchor();
	void toBlocks(Blocks::Cuboid b) const;

	void createSegments();

	bool hasSegments() const;
	const std::vector<std::unique_ptr<UniqueSegment>> &segments() const {return uniqueSegments_;}

	const Connector &front() const;
	const Connector &back() const;

	Container &owner_;
	bool anchor_;
	std::shared_ptr<Connection> connection_;
	bool has_extents_;
	Blocks::index_t relativePosition_, start_, end_;

	struct Less {
		using is_transparent = void;

		bool operator()( const Wire* const & lhs, const Wire* const & rhs) const
		{
			return lhs -> position() < rhs -> position();
		}

		bool operator()( const Wire* const & lhs, Blocks::index_t rhs) const
		{
			return lhs -> position() < rhs;
		}

		bool operator()( Blocks::index_t lhs, const Wire* const & rhs) const
		{
			return lhs < rhs -> position();
		}
	};

	using SortedCrossers = std::set<Wire *, Less>;

	const SortedCrossers &getSortedCrossers() const {return sorted_crossers_;}

	static void blocks(int height_offset, Blocks::Wall line_segment, Blocks::index_t position, Blocks::index_t length);
	Blocks::Wall segment(Blocks::Cuboid &blocks) const;

	void placeRepeater(Blocks::index_t position, bool reverse, Blocks &b) const;

protected:
	std::vector<Wire *> wiresCrossConnectedAt(Blocks::index_t pos) const;

	static Blocks::Line line(Blocks::Wall segment, Blocks::index_t height, Blocks::index_t position, Blocks::index_t length);

	void blocks_isolated(Blocks::Wall line_segment, Blocks::index_t position, Blocks::index_t length) const;
	void blocks_connected(Blocks::Wall line_segment, Blocks::index_t position, Blocks::index_t length) const;

	SortedCrossers sorted_crossers_;

public:
	Wire &operator=(const Wire &) = delete;
	Wire &operator=(Wire &&) = delete;
	Wire (const Wire &) = delete;

	friend std::ostream &operator<<(std::ostream &os, const Wire &w);

private:
	struct ConnectorLess {
		using is_transparent = void;

		ConnectorLess(const Wire &This);

		bool operator()(
				const std::shared_ptr<Connector> &lhs,
				const std::shared_ptr<Connector> &rhs
				) const;

		bool operator()(
				const std::shared_ptr<Connector> &lhs,
				Blocks::index_t rhs
				) const;

		bool operator()(
				Blocks::index_t lhs,
				const std::shared_ptr<Connector> &rhs
				) const;

	private:
		const Wire &this_;
	};

	UniqueSegment &addSegment(Blocks::index_t start, Blocks::index_t end,
			Connector *startSeg, Connector *endSeg);

	Connector *addConnector(Blocks::index_t pos);

	Connector &getConnectorAt(Blocks::index_t pos) const;
	const Connector &onlyConnector() const;

	std::vector<std::unique_ptr<UniqueSegment>> uniqueSegments_;
	std::set<std::shared_ptr<Connector>, ConnectorLess> connectors_;
};

std::ostream &operator<<(std::ostream &os, const Wire &w);

}
}

#endif // WIREBASE_H
