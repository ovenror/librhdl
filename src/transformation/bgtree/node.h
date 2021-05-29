#ifndef TM_NODE_H
#define TM_NODE_H

#include "horizontal.h"
#include "singlewire.h"
#include <vector>
#include <bitset>

namespace rhdl {
namespace TM {

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

	Blocks::index_t place(Blocks::index_t startPos);
	Blocks::index_t placeAt(Blocks::index_t pos);
	bool placeableAt(Blocks::index_t pos) const;
	bool outputPlaceableAt(Blocks::index_t pos) const;
	bool inputPlaceableAt(Blocks::index_t pos) const;
	void setPosition(Blocks::index_t pos);
	void applyToWires(std::function<void(Wire &)> f);

	Blocks::index_t ypos() const override;
	void toBlocks(Blocks::Cuboid b) const;

	const std::vector<Blocks::Vec> &getTorchPositions() const {return torchPositions_;}

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
	Blocks::index_t clearSpace(Blocks::index_t startPos);
	Blocks::index_t clearHeadSpace(Blocks::index_t startPos);
	Blocks::index_t clearFootSpace(Blocks::index_t startPos);

	void mkInverter(Blocks::Cuboid segment) const;

	bool isolatedCrossingByProximateCrosserBelow() const;
	bool connectedCrossingByProximateCrosserAbove() const;

private:
	void addTorchPos(Blocks::Vec v) const;

	NodeGroup *child_;
	mutable unsigned int depthAfter_;
	bool lowInverter_;
	mutable std::vector<Blocks::Vec> torchPositions_;
};

}}

#endif // NODE_H
