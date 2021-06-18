#include "nodegroup.h"
#include "node.h"
#include "layer.h"
#include "connection.h"
#include "treemodel.h"
#include <numeric>

namespace rhdl {

using blocks::Blocks;

namespace TM {

NodeGroup::NodeGroup(Layer &container, unsigned int index,
		Node *parent, Wire &manifold_cross)
	:
	  Horizontal(index), layer_(container), manifold_cross_(manifold_cross), parent_(nullptr)
{
	if (parent)
		setParent(*parent);
}

void NodeGroup::setParent(Node &parent)
{
	assert (!parent_);
	parent_ = &parent;
	parent.setChild(*this);
}

Node &NodeGroup::makeInverter()
{
	return makeNode_internal(true);
}

Node &NodeGroup::makePassThrough()
{
	return makeNode_internal(false);
}

Node &NodeGroup::makeNode_internal(bool invert)
{
	nodes_.push_back(std::make_unique<Node>(*this, nodes_.size(), manifold_cross_, invert));   
	return *nodes_.back();
}

Node &NodeGroup::insertPassThrough(unsigned int index)
{
	insert(std::make_unique<Node>(*this, index, manifold_cross_, false), nodes_);
	return *nodes_[index];
}

Blocks::index_t NodeGroup::placeNode(Node &node, Blocks::index_t startPos)
{
	if (nodes_.size() == 1 && parent_) {
		Blocks::index_t inputPos = parent_ -> output_.position();

		if (inputPos >= startPos && node.outputPlaceableAt(inputPos)) {
			return node.placeAt(inputPos);
		}
	}

	return node.place(startPos);
}

void NodeGroup::computeHorizontal()
{
	setPosition(nodes_[0] -> position());
	Node &last = *nodes_.back();
	width_ = last.position() + last.width() - position();
}

void NodeGroup::computeVertical()
{
	height_ = 0;

	for (auto &pnode : nodes_) {
		pnode -> computeVertical();
		height_ = std::max(height_, pnode -> height());
	}
}

}}
