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

	blocks::index_t place(blocks::index_t startPos);
	blocks::index_t placeAt(blocks::index_t pos);
	bool placeableAt(blocks::index_t pos) const;
	bool outputPlaceableAt(blocks::index_t pos) const;
	bool inputPlaceableAt(blocks::index_t pos) const;
	void setPosition(blocks::index_t pos);
	void applyToWires(std::function<void(Wire &)> f);

	blocks::index_t ypos() const override;
	void toBlocks(blocks::Cuboid b) const;

	const std::vector<blocks::Vec> &getTorchPositions() const {return torchPositions_;}

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
	blocks::index_t clearSpace(blocks::index_t startPos);
	blocks::index_t clearHeadSpace(blocks::index_t startPos);
	blocks::index_t clearFootSpace(blocks::index_t startPos);

	void mkInverter(blocks::Cuboid segment) const;

	bool isolatedCrossingByProximateCrosserBelow() const;
	bool connectedCrossingByProximateCrosserAbove() const;

private:
	void addTorchPos(blocks::Vec v) const;

	NodeGroup *child_;
	mutable unsigned int depthAfter_;
	bool lowInverter_;
	mutable std::vector<blocks::Vec> torchPositions_;
};

}}

#endif // NODE_H
