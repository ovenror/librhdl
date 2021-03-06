#ifndef NODEGROUP_H
#define NODEGROUP_H

#include "representation/netlist/netlist.h"
#include <vector>
#include "../tree/horizontal.h"
#include "../tree/node.h"

namespace rhdl {
namespace spatial {

class Layer;

class NodeGroup : public Horizontal
{
public:
	NodeGroup(Layer &container, unsigned int index, Node *parent, Wire &manifold_cross);

	const NodeGroup *getNodeGroup() const override {return this;}
	NodeGroup *getNodeGroup() override {return this;}

	const Layer* getLayer() const override {return &layer_;}
	Layer *getLayer() override {return &layer_;}

	void setParent(Node &parent);
	Node *getParent() {return parent_;}

	Node &makeInverter();
	Node &makePassThrough();
	Node &insertPassThrough(unsigned int index = 0);

	blocks::index_t placeNode(Node &node, blocks::index_t startPos);

	void computeHorizontal();
	void computeVertical();

	using NodePtr = std::unique_ptr<Node>;
	using NodeContainer = std::vector<NodePtr>;

	Layer &layer_;
	NodeContainer nodes_;
	Wire &manifold_cross_;

private:   
	Node &makeNode_internal(bool invert);
	blocks::index_t placeNode_internal(Node &node);

	Node *parent_;
};

}}

#endif // NODEGROUP_H
