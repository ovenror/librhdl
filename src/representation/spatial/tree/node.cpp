#include "../tree/node.h"

#include <memory>
#include <cassert>
#include "../tree/connection.h"
#include "../tree/layer.h"
#include "../tree/nodegroup.h"
#include "../tree/wires.h"

namespace rhdl {
namespace spatial {

Node::Node(NodeGroup &container, unsigned int index, Wire &input_manifold, bool invert)
	: Horizontal(index), node_group_(container),
	  invert_(invert), output_(*this, true),
	  output_anchor_(*this, false, output_.connection_, true),
	  input_(*this, true, input_manifold.connection_),
	  input_anchor_(*this, false, input_.connection_, true),
	  passThrough_(*this, true, false),
	  shortcut_(false), straightTorch_(false), leftTorch_(false),
	  rightTorch_(false), child_(nullptr), depthAfter_(0),
	  lowInverter_(false)
{
	Layer &layer = node_group_.layer_;

	output_.addCrosser(layer.cross());
	output_.addCrosser(output_anchor_);
	input_.addCrosser(input_anchor_);
	input_.addCrosser(layer.crossBelow());

	if (!invert) {
		passThrough_.addCrosser(input_anchor_);
		passThrough_.addCrosser(output_anchor_);

		passThrough_.connect(input_anchor_);
		passThrough_.connect(output_anchor_);
	}
}

NodeGroup *Node::child() const
{
	return child_;
}

void Node::setChild(NodeGroup &child)
{
	assert (!child_);
	child_ = &child;
}

unsigned int Node::depthAfter()
{
	if (!child())
		return 0;

	if (depthAfter_)
		return depthAfter_;

	unsigned int max_child_depth_after = 0;
	for (auto &pnode : child() -> nodes_) {
		depthAfter_ = std::max(max_child_depth_after, pnode -> depthAfter());
	}

	depthAfter_ = max_child_depth_after + 1;

	return depthAfter_;
}

void Node::preComputeSpatial() {
	output_.relativePosition_ = 0;
	input_.relativePosition_ = 0;

	output_anchor_.relativePosition_ = 2;
	input_anchor_.relativePosition_ = 0;

	passThrough_.relativePosition_ = 0;

	width_ = 1;
	height_ = 3;
}

bool Node::placeableAt(blocks::index_t pos) const
{
	return inputPlaceableAt(pos) && outputPlaceableAt(pos);
}

bool Node::outputPlaceableAt(blocks::index_t pos) const
{
	Layer &layer = node_group_.layer_;
	const Wires &lcross = layer.cross();
	return output_.mayCross(lcross, pos);
}

bool Node::inputPlaceableAt(blocks::index_t pos) const
{
	Layer &layer = node_group_.layer_;
	const Wires &cross_below = layer.crossBelow();
	return input_.mayCross(cross_below, pos);
}

void Node::computeVertical()
{
	if (shortcut_)
		output_anchor_.relativePosition_ = 1;
}

bool Node::inputIsConnected() const
{
	return !invert_ || !isolatedCrossingByProximateCrosserBelow();
}

blocks::index_t Node::place(blocks::index_t startPos)
{
	Layer &layer = node_group_.layer_;
	assert (startPos == layer.asContainer().width() + 1);

	blocks::index_t skipPos = startPos + 2;

	if (child() && layer.above() -> asContainer().width() + 1 == skipPos) {
		assert (outputPlaceableAt(skipPos));
		return placeAt(skipPos);
	}

	return placeAt(clearSpace(startPos));
}

blocks::index_t Node::placeAt(blocks::index_t pos)
{
	setPosition(pos);
	return pos + width_;
}

blocks::index_t Node::clearSpace(blocks::index_t startPos)
{
	blocks::index_t pos = startPos;

	assert (!input_.hasPosition());
	assert (!output_.hasPosition());

	for (; !placeableAt(pos); pos += 2);

	return pos;
}

blocks::index_t Node::clearFootSpace(blocks::index_t startPos)
{
	Layer &layer = node_group_.layer_;
	blocks::index_t pos = startPos;
	const Wires &cross_below = layer.crossBelow();

	assert (!input_.hasPosition());
	for (; !input_.mayCross(cross_below, pos); pos += 2);

	return pos;
}

blocks::index_t Node::clearHeadSpace(blocks::index_t startPos)
{
	Layer &layer = node_group_.layer_;
	blocks::index_t pos = startPos;
	const Wires &lcross = layer.cross();

	assert (!output_.hasPosition());
	for (; !output_.mayCross(lcross, pos); pos += 2);

	return pos;
}


void Node::setPosition(blocks::index_t pos)
{
	Element::setPosition(pos);

	input_.computeAbsolutePosition(pos);
	output_.computeAbsolutePosition(pos);
	passThrough_.computeAbsolutePosition(pos);
}

void Node::applyToWires(std::function<void (Wire &)> f)
{
	f(input_anchor_);
	f(input_);
	f(output_);
	f(output_anchor_);

	if (!invert_)
		f(passThrough_);
}

bool Node::isolatedCrossingByProximateCrosserBelow() const
{
	const Layer *layer = getLayer();
	assert (layer);

	const Wires &crossBelow = layer -> crossBelow();

	for (const Wire *w : crossBelow.top()) {
		if (input_.crosses(*w) && !input_.isConnected(*w) && !w -> anchor_)
			return true;
	}

	return false;
}

bool Node::connectedCrossingByProximateCrosserAbove() const
{
	const Layer *layer = getLayer();
	assert (layer);

	const Wires &crossAbove = layer -> cross();

	for (const Wire *w : crossAbove.bottom()) {
		if (output_.crosses(*w) && output_.isConnected(*w) && !w -> anchor_)
			return true;
	}

	return false;
}

void Node::addTorchPos(blocks::Vec v) const
{
	v[2] += ypos();
	torchPositions_.push_back(v);
}

blocks::index_t Node::ypos() const
{
	return node_group_.ypos();
}

void Node::toBlocks(blocks::Cuboid b) const
{
	blocks::Cuboid target = blocks::slice3(b, {0, 0, ypos()}, {0, 0, height_});

	if (invert_)
		mkInverter(target);
}

void Node::mkInverter(blocks::Cuboid segment) const
{
	using blocks::Block;
	using blocks::Direction;
	using blocks::RIGHT;
	using blocks::FORWARD;
	using blocks::LEFT;
	using blocks::BACKWARD;

	blocks::index_t x = xpos();
	blocks::index_t r = xpos() + 1;
	blocks::index_t l = xpos() - 1;
	blocks::index_t baseline = lowInverter_ ? 1 : 2;

	if (!shortcut_ || straightTorch_) {
		blocks::Vec v{baseline, x, 2};
		addTorchPos(v);
		segment[baseline][x][2] = Block(Block::TORCH, FORWARD);
	}

	if (leftTorch_) {
		blocks::Vec v{baseline, l, 1};
		addTorchPos(v);
		segment[baseline][l][1] = Block(Block::TORCH, LEFT);
	}

	if (rightTorch_) {
		blocks::Vec v{baseline, r, 1};
		addTorchPos(v);
		segment[baseline][r][1] = Block(Block::TORCH, RIGHT);
	}

	segment[baseline][x][1] = Block::OPAQUE;

	segment[baseline + 1][x][0] = Block::UNSET;
	segment[baseline][x][0] = Block::REDSTONE;
	segment[baseline -1][x][0] = Block::OPAQUE;
}

bool Node::canHaveLowInverter() const
{
	assert (invert_);

	if (connectedCrossingByProximateCrosserAbove())
		return false;

	// actually it does work, if this is the only output on this connection
	if (leftTorch_ + rightTorch_ + straightTorch_ > 1) {
		assert(shortcut_);
		return false;
	}

	return true;
}

void Node::useLowInverter()
{
	assert (invert_);
	lowInverter_ = true;
}

}}
