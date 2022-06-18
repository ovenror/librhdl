#ifndef TM_NODE_H
#define TM_NODE_H

#include <vector>
#include <bitset>
#include "../tree/horizontal.h"
#include "../tree/singlewire.h"

namespace rhdl {
namespace spatial {

class NodeGroup;

class Node : public Horizontal
{
public:
	Node(NodeGroup &container, unsigned int index, Wire &input_manifold, bool invert);

	const Node *getNode() const override {return this;}
	Node *getNode() override {return this;}

	const NodeGroup *getNodeGroup() const override {return &node_group_;}
	NodeGroup *getNodeGroup() override {return &node_group_;}

	void preComputeSpatial();
	bool inputIsConnected() const;
	bool canHaveLowInverter() const;
	void useLowInverter();
	void computeVertical();

	NodeGroup *child() const;
	void setChild(NodeGroup &child);
	unsigned int depthAfter();

	bool backwards() const {return false;}

	blocks::Blocks::index_t place(blocks::Blocks::index_t startPos);
	blocks::Blocks::index_t placeAt(blocks::Blocks::index_t pos);
	bool placeableAt(blocks::Blocks::index_t pos) const;
	bool outputPlaceableAt(blocks::Blocks::index_t pos) const;
	bool inputPlaceableAt(blocks::Blocks::index_t pos) const;
	void setPosition(blocks::Blocks::index_t pos);
	void applyToWires(std::function<void(Wire &)> f);

	blocks::Blocks::index_t ypos() const override;
	void toBlocks(blocks::Blocks::Cuboid b) const;

	const std::vector<blocks::Blocks::Vec> &getTorchPositions() const {return torchPositions_;}

	NodeGroup &node_group_;
	bool invert_;
	SingleWire output_;
	SingleWire output_anchor_;
	SingleWire input_;
	SingleWire input_anchor_;
	SingleWire passThrough_;
	bool shortcut_;
	bool straightTorch_;
	bool leftTorch_;
	bool rightTorch_;

protected:
	blocks::Blocks::index_t clearSpace(blocks::Blocks::index_t startPos);
	blocks::Blocks::index_t clearHeadSpace(blocks::Blocks::index_t startPos);
	blocks::Blocks::index_t clearFootSpace(blocks::Blocks::index_t startPos);

	void mkInverter(blocks::Blocks::Cuboid segment) const;

	bool isolatedCrossingByProximateCrosserBelow() const;
	bool connectedCrossingByProximateCrosserAbove() const;

private:
	void addTorchPos(blocks::Blocks::Vec v) const;

	NodeGroup *child_;
	mutable unsigned int depthAfter_;
	bool lowInverter_;
	mutable std::vector<blocks::Blocks::Vec> torchPositions_;
};

}}

#endif // NODE_H
