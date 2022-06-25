#ifndef MUTABLELAYER_H
#define MUTABLELAYER_H

#include "representation/netlist/netlist.h"

#include "util/catiterator.h"

#include "boost/iterator/transform_iterator.hpp"
#include "../tree/container.h"
#include "../tree/divisivewires.h"
#include "../tree/layer.h"
#include "../tree/nodegroup.h"
#include "../tree/wire.h"

namespace rhdl {
namespace spatial {

class TreeModel;

class MutableLayer : public Container, public Layer
{
public:
	MutableLayer(TreeModel &container, unsigned int index);

	const Container &asContainer() const override {return *this;}

	const Layer *getLayer() const override {return this;}
	Layer *getLayer() override {return this;}

	const TreeModel& getModel() const override {return model_;}
	//TreeModel& getModel() override {return model_;}

	NodeGroup &makeNodeGroup(Node *parent, Wire &manifold_cross);
	NodeGroup &insertNodeGroup(Node *parent, Wire &manifold_cross, unsigned int index = 0);
	Node &chainPassThrough(Node &parent, Wire &parent_output);

	void preComputeSpatial();

	void placeNode(Node &node);

	void createShortcuts();
	bool needsExtraBlockAtBottom();
	void computeHorizontalCollectedWiresPosition();
	void computeVertical();

	void applyToWires(std::function<void(Wire &)> f);

	void toBlocks(blocks::Cuboid b) const;

	MutableLayer *mabove() const;
	MutableLayer *mbelow() const;

	const Layer *above() const override {return mabove();}
	const Layer *below() const override {return mbelow();}

	const Wires &cross() const override {return cross_;}
	Wires &cross() {return cross_;}

	const Wires &crossBelow() const;
	Wires &crossBelow();

	const auto &nodegroups() const {return nodegroups_;}
	auto &nodegroups() {return nodegroups_;}

	std::vector<const Wire *> inputs(const std::function<bool (const Wire &)> &predicate) const;
	std::vector<const Wire *> upperLayerInputs(const std::function<bool(const Wire &)> &predicate) const;

	blocks::index_t xpos() const override;
	blocks::index_t ypos() const override;

	using NodeGroupPtr = std::unique_ptr<NodeGroup>;
	using NodeGroupContainer = std::vector<NodeGroupPtr>;
	using NodeContainer = NodeGroup::NodeContainer;
	using UnwrapType = std::function<NodeContainer &(const NodeGroupPtr &)>;
	using GroupNodesIterator = boost::transform_iterator<UnwrapType, NodeGroupContainer::const_iterator>;
	using NodesIterator = CatIterator<GroupNodesIterator>;
	using NodesIterable =  CatGenerator<GroupNodesIterator>;

	GroupNodesIterator node_containers_begin() const;
	GroupNodesIterator node_containers_end() const;

	NodesIterable nodes() const;

private:
	blocks::index_t tryCreateShortcut(
			NodesIterator from, NodesIterator to,
			blocks::index_t rangeFrom,
			blocks::index_t lastNodeOutputPos,
			blocks::index_t rangeEnd);
	std::vector<const Wire *> getConnectedCrossers(const Connection &connection) const;

	TreeModel &model_;
	DivisiveWires cross_;
	NodeGroupContainer nodegroups_;
	std::vector<std::unique_ptr<SingleWire> > shortcuts_;
};


}


}

#endif // MUTABLELAYER_H
