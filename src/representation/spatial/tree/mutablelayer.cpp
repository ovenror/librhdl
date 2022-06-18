#include "../tree/mutablelayer.h"

#include "../tree/treemodel.h"
#include "util/iterator_pair.h"

namespace rhdl {

using blocks::Blocks;

namespace spatial {

MutableLayer::MutableLayer(TreeModel &container, unsigned int index)
	: Container(index), model_(container), cross_(*this, false)
{}

NodeGroup &MutableLayer::makeNodeGroup(Node *parent, Wire &manifold_cross)
{
	nodegroups_.emplace_back(std::make_unique<NodeGroup>(*this, nodegroups_.size(), parent, manifold_cross));
	return *nodegroups_.back();
}

NodeGroup &MutableLayer::insertNodeGroup(Node *parent, Wire &manifold_cross, unsigned int index)
{
	insert(std::make_unique<NodeGroup>(*this, index, parent, manifold_cross), nodegroups_);
	return *nodegroups_[index];
}

Node &MutableLayer::chainPassThrough(Node &parent, Wire &parent_output)
{
	//assert (parent.index_ == 0);
	assert (!parent.child());
	assert (parent.getLayer() -> asContainer().index() == index() - 1);

	unsigned int index = 0;
	const NodeGroup &parentGroup = parent.node_group_;

	for (const auto &png : nodegroups_) {
		Node *before = png -> getParent();

		if (!before)
			continue;

		NodeGroup &beforeGroup = before -> node_group_;

		if (beforeGroup.index() > parentGroup.index())
			break;

		if (beforeGroup.index() == parentGroup.index())
			if (before -> index() > parent.index())
				break;

		index = png -> index() + 1;
		break;
	}

	Wire &manifold = crossBelow().findOrMake(parent_output.connection_);
	NodeGroup &ng = insertNodeGroup(&parent, manifold, index);

	return ng.makePassThrough();
}


void MutableLayer::createShortcuts()
{
	auto ipfrom = nodes().begin();
	const auto end = nodes().end();
	Blocks::index_t rangeBegin = 0;
	Blocks::index_t preRangeEnd = 0;

	while (ipfrom != end) {
		auto ipto = ipfrom;
		Wire &ipfrom_out = (*ipfrom) -> output_;
		const Connection &connection = *ipfrom_out.connection_;
		Blocks::index_t lastNodeOutputPos;

		for (; ipto != end && (*ipto) -> output_.isConnected(connection); ++ipto) {
			lastNodeOutputPos = (*ipto) -> output_.position();
		}

		if (ipto == end)
			preRangeEnd = model_.width(); //oof
		else
			preRangeEnd = (*ipto) -> output_.position() - 1;

		if ((*ipfrom) -> output_.position() < rangeBegin)
			rangeBegin = preRangeEnd;
		else
			rangeBegin = tryCreateShortcut(ipfrom, ipto, rangeBegin, lastNodeOutputPos, preRangeEnd);

		rangeBegin += 2;
		ipfrom = ipto;
	}
#if 0
	for (auto &pwire : shortcuts_) {
		pwire -> computeExtents();
		pwire -> tryBecomeAnchor();
		assert (!pwire -> anchor_);
	}
#endif
}

bool MutableLayer::needsExtraBlockAtBottom()
{
	std::vector<Node *> disconnectedNodes;
	bool lowInvertersPossible = true;

	for (const auto &pnode : nodes()) {
		if (pnode -> inputIsConnected())
			continue;

		disconnectedNodes.push_back(pnode.get());

		if (!pnode -> canHaveLowInverter())
			lowInvertersPossible = false;
	}

	if (disconnectedNodes.empty())
		return false;

	if (!lowInvertersPossible)
		return true;

	for (Node *n : disconnectedNodes)
		n -> useLowInverter();

	return false;
}

/*
 * shitty... could be avoided by explicitly storing a left and a right connector
 * (upper and lower) for each wire in SortedCrossers
 *
 * not needed, connections from above always happen via newly created children
 * or a new passthrough in the existing child nodegroup
 */
std::vector<const Wire *> MutableLayer::inputs(const std::function<bool (const Wire &)> &predicate) const
{
	std::vector<const Wire *> result;

	for (const auto &pnode : nodes()) {
		SingleWire &input = pnode -> input_;

		if (predicate(input))
			result.push_back(&input);
	}

	return result;
}

std::vector<const Wire *> MutableLayer::upperLayerInputs(const std::function<bool (const Wire &)> &predicate) const
{
	std::vector<const Wire *> result;

	const Layer *abov = above();

	if (abov)
		return abov -> inputs(predicate);

	for (const auto &pnode : nodes()) {
		SingleWire &output = pnode -> output_;

		if (!model_.isTopInterfaceWire(output))
			continue;

		if (predicate(output))
			result.push_back(&output);
	}

	return result;
}


Blocks::index_t MutableLayer::tryCreateShortcut(
		MutableLayer::NodesIterator from, MutableLayer::NodesIterator to,
		Blocks::index_t rangeBegin, Blocks::index_t lastNodeOutputPos,
		Blocks::index_t rangeEnd)
{
	Blocks::index_t result = lastNodeOutputPos;
	auto numOutputs = distance(from, to);
	assert (numOutputs >= 1);

	/*
	 * check if there are other outputs on this layer with this connection
	 */
	const Connection &connection = *((*from) -> output_.connection_);

	for (NodesIterator ipnode = nodes().begin(); ipnode != from; ++ipnode) {
		if ((*ipnode) -> output_.isConnected(connection))
			return result;
	}

	for (NodesIterator ipnode = to; ipnode != nodes().end(); ++ipnode) {
		if ((*ipnode) -> output_.isConnected(connection))
			return result;
	}

	/*
	 * check number of passthroughs / inverters
	 */
	unsigned int numPassThru = 0;
	unsigned int numInverters = 0;
	for (auto ipnode = from; ipnode != to; ++ipnode) {
		if ((*ipnode) -> invert_)
			++numInverters;
		else
			++numPassThru;
	}

	assert (numPassThru + numInverters == numOutputs);

	if (numPassThru == 0 || numInverters == 0)
		return result;

	// make sure nodes are untouched up to now
	assert (std::none_of(from, to, [](const auto &pnode) {return pnode -> shortcut_;}));


	/*
	 * - Every upper wire has to be in range.
	 * - Calculate actual range end.
	 * - Mark necessary straight torches for shortest cut.
	 */
	const std::vector<const Wire *> upperWires = Layer::upperLayerInputs(connection);
	Blocks::index_t upperPos = -1;

	NodesIterator ipnode = from;

	for (const Wire *wire : upperWires) {      
		assert (upperPos < wire -> position());
		upperPos = wire -> position();

		if (upperPos < rangeBegin || upperPos >= rangeEnd)
			return result;

		for (; ipnode != to && (*ipnode) -> output_.position() < upperPos; ++ipnode);

		if (ipnode == to || (*ipnode) -> output_.position() != upperPos)
			continue;

		Node &node = **ipnode;
		++ipnode;

		if (node.invert_)
			node.straightTorch_ = true;
	}

	result = std::max(lastNodeOutputPos, upperPos);

	/*
	 * Cut shortcut wire from wire collection.
	 */
	Wire &cross = cross_.find(connection);
	shortcuts_.push_back(cross_.release(cross));
	SingleWire &shortcut = *shortcuts_.back();
	assert (&shortcut.owner_ == this);
	shortcut.relativePosition_ = 1;

	/*
	 * Configure nodes.
	 */
	ipnode = from;
	Node &node = **ipnode;
	node.shortcut_ = 2;
	node.rightTorch_ = true;
	++ipnode;

	for (; ipnode != to;) {
		Node &node = **ipnode;
		node.shortcut_ = 2;
		node.leftTorch_ = true;

		++ipnode;

		if (ipnode != to)
			node.rightTorch_ = true;
	}

	return result;
}

void MutableLayer::preComputeSpatial()
{
	assert(!nodegroups_.empty());

	for (auto &pnode : nodes()) {
		pnode -> preComputeSpatial();
	}

	width_ = -1;
}

void MutableLayer::placeNode(Node &node)
{
	width_ = node.node_group_.placeNode(node, width_ + 1);
}

void MutableLayer::computeHorizontalCollectedWiresPosition()
{
	cross_.computePositions();
}

void MutableLayer::computeVertical()
{
	Blocks::index_t max_ng_height = 0;

	for (auto &png : nodegroups_) {
		png -> computeVertical();
		max_ng_height = std::max(max_ng_height, png -> height());
	}


	if (cross_.noNonAnchors()) {
		height_ = max_ng_height;
		cross_.setPosition(position() + max_ng_height - 1);
		return;
	}

	cross_.setPosition(position() + max_ng_height);
	height_ = max_ng_height + 1 + cross_.maxRelPos();
}

void MutableLayer::applyToWires(std::function<void (Wire &)> f)
{
	cross_.applyToWires(f);

	for (auto &pnode : nodes()) {
		pnode -> applyToWires(f);
	}

	for (auto &pwire : shortcuts_)
		f(*pwire);
}

MutableLayer *MutableLayer::mabove() const
{
	if (index() == model_.mlayers().size() - 1)
		return nullptr;

	return model_.mlayers()[index() + 1].get();
}

MutableLayer *MutableLayer::mbelow() const
{
	if (index() == 0)
		return nullptr;

	return model_.mlayers()[index() - 1].get();
}

const Wires &MutableLayer::crossBelow() const
{
	const Layer *lbelow = below();
	return lbelow ? lbelow -> cross() : model_.lowerCross();
}

Wires &MutableLayer::crossBelow()
{
	MutableLayer *lbelow = mbelow();
	return lbelow ? lbelow -> cross() : model_.lowerCross();
}

Blocks::index_t MutableLayer::xpos() const
{
	return 0;
}

Blocks::index_t MutableLayer::ypos() const
{
	return position();
}

static MutableLayer::NodeContainer &unwrap(const MutableLayer::NodeGroupPtr &png)
{
	assert (!png -> nodes_.empty());
	return png -> nodes_;
}

MutableLayer::GroupNodesIterator MutableLayer::node_containers_begin() const
{
	return boost::make_transform_iterator(nodegroups_.cbegin(), unwrap);
}

MutableLayer::GroupNodesIterator MutableLayer::node_containers_end() const
{
	return boost::make_transform_iterator(nodegroups_.cend(), unwrap);
}

MutableLayer::NodesIterable MutableLayer::nodes() const
{
	return CatGenerator<GroupNodesIterator>(node_containers_begin(), node_containers_end());
}

void MutableLayer::toBlocks(Blocks::Cuboid b) const
{
	for (auto &pwire : shortcuts_) {
		Wire &wire = *pwire;
		Blocks::index_t start = wire.start_;

		Wire::blocks(-1, wire.segment(b), start, wire.end_ - start);
	}

	for (auto &node : nodes()) {
		node -> toBlocks(b);
	}
}

}}

