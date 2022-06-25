#include "../tree/treemodel.h"

#include "entity/entity.h"

#include "simulation/blockssim.h"

#include "interface/isingle.h"

#include "representation/netlist/netlist.h"
#include "representation/blocks/blocks.h"

#include "util/iterable.h"

#include <functional>
#include <stack>
#include <map>
#include <set>
#include <unordered_set>
#include "../tree/connection.h"
#include "../tree/connector.h"
#include "../tree/mutablelayer.h"
#include "../tree/node.h"
#include "../tree/nodegroup.h"
#include "../tree/wire.h"

namespace rhdl {

class ISingle;

using blocks::Blocks;
using blocks::BlocksSim;

using netlist::Netlist;
using netlist::VertexRef;
using netlist::EdgeRef;

namespace spatial {

struct VertexInfo {
	VertexInfo(Wire &pinput) : input(pinput), parent_node(nullptr) {}

	Wire &input;
	Node *parent_node;
};

struct ConstructionData_References {
	ConstructionData_References(
			const Netlist::Graph &pgraph, const Netlist::InterfaceMap &pnl_iface,
			const std::vector<const ISingle *> &pbottom_ifs,
			const std::vector<const ISingle *> &ptop_ifs)
		:
			graph(pgraph), nl_iface(pnl_iface), bottom_ifs(pbottom_ifs), top_ifs(ptop_ifs)
	{}

	const Netlist::Graph &graph;
	const Netlist::InterfaceMap &nl_iface;
	const std::vector<const ISingle *> &bottom_ifs, &top_ifs;
};

struct ConstructionData : public ConstructionData_References {
	ConstructionData(
			const Netlist::Graph &graph, const Netlist::InterfaceMap &nl_iface,
			const std::vector<const ISingle *> &bottom_ifs,
			const std::vector<const ISingle *> &top_ifs)
		:
			ConstructionData_References(graph, nl_iface, bottom_ifs, top_ifs)
	{}

	std::vector<VertexRef> lower_vertices;
	std::vector<VertexRef> upper_vertices;
	std::map<VertexRef, VertexInfo> known_vertices;
	std::map<EdgeRef, Node *> known_edges;
};

struct ConstructionData_AllReferences : public ConstructionData_References {
	ConstructionData_AllReferences(ConstructionData &data)
		: ConstructionData_References((ConstructionData_References &) data),
		  lower_vertices(data.lower_vertices), upper_vertices(data.upper_vertices),
		  known_vertices(data.known_vertices), known_edges(data.known_edges)
	{}

	std::vector<VertexRef> &lower_vertices;
	std::vector<VertexRef> &upper_vertices;
	std::map<VertexRef, VertexInfo> &known_vertices;
	std::map<EdgeRef, Node *> &known_edges;
};


struct IFaceData : public ConstructionData_AllReferences {
	IFaceData(const ConstructionData_AllReferences &data,
					const ISingle *piface)
		:
		  ConstructionData_AllReferences(data), iface(piface)
	{}

	const ISingle *iface;
};

struct VertexData : public LayerData {
	VertexData(const LayerData &data, MutableLayer &ul, VertexRef v)
		: LayerData(data), upper_layer(ul), vertex(v)
	{}

	MutableLayer &upper_layer;
	VertexRef vertex;
};

struct EdgesData : public VertexData {
	using Super = VertexData;

	EdgesData(
			const Super &data, NodeGroup &ng,
			const Netlist::Graph::OutEdgeIterator &ebegin,
			const Netlist::Graph::OutEdgeIterator &eend) :
		Super(data), nodegroup(ng), edges_begin(ebegin), edges_end(eend)
	{}

	NodeGroup &nodegroup;
	Netlist::Graph::OutEdgeIterator edges_begin;
	Netlist::Graph::OutEdgeIterator edges_end;
};

struct EdgeData : public EdgesData {
	using Super = EdgesData;

	EdgeData(const Super &data, EdgeRef e)
		: Super(data), edge(e)
	{}

	EdgeRef edge;
};

struct LeafData : public EdgeData {
	using Super = EdgeData;

	LeafData(const Super &data, VertexInfo &known) :
		Super(data), known_vertex(known) {}

	VertexInfo &known_vertex;
};

static std::vector<const ISingle *> ifilter(const Netlist::InterfaceMap &nli, Interface::Direction dir)
{
	std::vector<const ISingle *> result;

	for (auto &kv : nli) {
		const ISingle *iface = kv.first;

		if (iface -> direction() == dir)
			result.push_back(iface);
	}

	return result;
}

TreeModel::TreeModel(
		const Entity &entity, const Representation *parent,
		const Timing *timing,
		const std::vector<const ISingle *> &lower,
		const std::vector<const ISingle *> &upper) :
	RepresentationBase(entity, parent, timing),
	Container(0), bottom_anchors_(*this, false, true), bottom_(*this, true),
	lower_cross_(*this, false), top_anchors_(*this, false, true)
{
	bottom_.addCrosser(lower_cross_);
	bottom_.addCrosser(bottom_anchors_);
}

TreeModel::TreeModel(const Entity &e)
	: TreeModel(e, nullptr, &e.addTiming())
{}

TreeModel::TreeModel(
		const Entity &entity, const Representation *parent,
		const Timing *timing)
	: TreeModel(entity, parent, timing, {}, {})
{}

TreeModel::~TreeModel() {}

std::unique_ptr<TreeModel> TreeModel::make(const netlist::Netlist &source)
{
	return make(source,
			ifilter(source.ifaceMap(), SingleDirection::IN),
			ifilter(source.ifaceMap(), SingleDirection::OUT));
}

std::unique_ptr<TreeModel> TreeModel::make(
		const netlist::Netlist &netlist,
		const std::vector<const ISingle*> &lower,
		const std::vector<const ISingle*> &upper)
{
	std::unique_ptr<TreeModel> result;
	const auto &entity = netlist.entity();
	auto source = std::cref(netlist);
	size_t count = 0;

	while (true) {
		result = std::make_unique<TreeModel>(
				entity, &source.get(), source.get().timing(), lower, upper);
		auto &model = *result;

#if 0
		std::stringstream filename;
		filename << entity.name() << count << ".dot";
		std::ofstream dotfile(filename.str());
		dotfile << source.get().graph();
#endif

		auto vertexMap = model.createModel(source, lower, upper);
		model.computeSpatial();
		model.createSegments();

		auto unsynthesizableVertices = model.fixBrokenLinks(vertexMap);

		if (unsynthesizableVertices.empty())
			break;

		//for (auto v : unsynthesizableVertices)
		//	std::cerr << "split " << v << std::endl;

		source = entity.addRepresentation(Netlist(source, unsynthesizableVertices));
		++count;
	}

	return std::move(result);
}


std::map<const Connection *, netlist::VertexRef> TreeModel::createModel(
		const netlist::Netlist &netlist,
		const std::vector<const ISingle*> &lower,
		const std::vector<const ISingle*> &upper)
{
	std::map<const Connection *, netlist::VertexRef> vertexMap;
	ConstructionData data(netlist.graph(), netlist.ifaceMap(), lower, upper);

	processBottomIFaces(BottomIFacesData(data));
	processLooseVertices(LooseVerticesData(data));

	while (!isTopLayer(data)) {
		nextLayer(data);
		processLayer(LayerData(data));
	}

	processTopIFaces(TopIFacesData(data));

	for (const auto &kv : data.known_vertices) {
		VertexRef vertex = kv.first;
		const VertexInfo &info = kv.second;
		Connection *connection = info.input.connection_.get();
		vertexMap[connection] = vertex;
	}

	return vertexMap;
}

void TreeModel::toBlocks(blocks::Cuboid b) const
{
	applyToWires([&](Wire &w){w.toBlocks(b);});

	for (auto &player : layers_) {
		player -> toBlocks(b);
	}
}

void TreeModel::toInterface(Blocks::InterfaceMap &interface) const
{
	for (auto &kv : interface_) {
		const ISingle *iface = kv.first;
		const Wire &wire = *kv.second;

		assert(wire.vertical());

		bool bottom = wire.collection() == &bottom_;

		blocks::index_t yPos = bottom ? 0 : height_ - 1;
		blocks::index_t height = 1;

		if (bottom && layers_[0] -> ypos() == 0)
			height = 2;

		auto crossers = wire.getSortedCrossers();
		auto ipcrosser = crossers.find(yPos);

		if (ipcrosser != crossers.end()) {
			const Wire &crosser = **ipcrosser;

			if (!crosser.anchor_ && crosser.isConnected(wire))
				height = 2;
		}

		interface[iface] = {height, wire.position(), yPos};

		//std::cerr << "iface @ " << height << "," << wire.getPosition() << "," << yPos << std::endl;
	}
}

void TreeModel::preComputeSpatial()
{
	for (auto &player : layers_) {
		player -> preComputeSpatial();
	}
}

void TreeModel::placeBottomWires()
{
	std::set<Wire *> loose_inputs;

	for (auto &pwire : bottom_)
		loose_inputs.insert(pwire.get());

	for (auto &png : layers_[0] -> nodegroups()) {
		NodeGroup &ng = *png;
		auto xpos = ng.position();

		for (Wire *bottom_input : bottom_.findConnected(*ng.manifold_cross_.connection_)) {
			if (!bottom_input -> mayCross(lower_cross_, xpos))
				break;

			bottom_input -> setPosition(xpos);
			loose_inputs.erase(bottom_input);
			xpos += 2;
		}
	}

	auto ipwire = loose_inputs.begin();
	for (auto xpos = 0; ipwire != loose_inputs.end(); xpos += 2) {
		Wire &wire = **ipwire;

		if (!wire.mayCross(lower_cross_, xpos))
			continue;

		wire.setPosition(xpos);
		++ipwire;
	}
}

void TreeModel::placeNodes()
{
	std::stack<Node *> stack;

	unsigned int layer_idx = 0;
	unsigned int group_idx = 0;
	unsigned int node_idx = 0;

	do {
		MutableLayer &layer = *layers_[layer_idx];
		NodeGroup &group = *layer.nodegroups()[group_idx];

		for (; node_idx < group.nodes_.size(); ++node_idx) {
			Node &node = *group.nodes_[node_idx];
			const NodeGroup *child = node.child();

			layer.placeNode(node);

			if (child) {
				stack.push(&node);
				++layer_idx;
				group_idx = child -> index();
				node_idx = 0;
				break;
			}
		}

		if (node_idx < group.nodes_.size())
			continue;

		NodeGroup *continuedGroup = &group;
		do {
			continuedGroup -> computeHorizontal();

			if (layer_idx == 0) {
				assert (stack.empty());
				continuedGroup = nullptr;
				break;
			}

			assert (!stack.empty());
			Node *pnode = stack.top();
			stack.pop();

			--layer_idx;
			continuedGroup = pnode -> getNodeGroup();
			group_idx = continuedGroup -> index();
			node_idx = pnode -> index();
			assert (continuedGroup -> nodes_[node_idx].get() == pnode);
		}
		while (++node_idx == continuedGroup -> nodes_.size());

		if (continuedGroup)
			continue;

		assert (layer_idx == 0);
		++group_idx;
		node_idx = 0;
	}
	while (layer_idx || group_idx < layers_[0] -> nodegroups().size());
}

void TreeModel::computeWidth()
{
	blocks::index_t max_layer_width = 0;

	for (auto &player : layers_) {
		max_layer_width  = std::max(max_layer_width , player -> width());
	}

	width_ = std::max(max_layer_width, bottom_.maxRelPos() + 1);
}

void TreeModel::computeHorizontalCollectedWiresPosition()
{
	lower_cross_.computePositions();

	for (const auto &player : layers_) {
		player -> computeHorizontalCollectedWiresPosition();
	}
}

void TreeModel::computeSpatial()
{
	preComputeSpatial();
	placeNodes();
	placeBottomWires();
	computeWidth();    
	//createShortcuts();
	applyToWires([](Wire &w){if (!w.vertical()) w.computeExtents();});
	applyToWires([](Wire &w){if (!w.vertical()) w.tryBecomeAnchor();});
	computeHorizontalCollectedWiresPosition();
	computeVertical();
}

bool TreeModel::hasBrokenLinks(const spatial::ConnectionLinks &assessment) const
{
	for (const auto &kv : assessment) {
		if (!kv.second.second.empty())
			return true;
	}

	return false;
}

#if 0
bool TreeModel::hasBrokenLinks(const Blocks &b)
{
	for (const auto &kv : assessLinks(b)) {
		if (!kv.second.second.empty())
			return true;
	}

	return false;
}
#endif

void TreeModel::applyToWires(std::function<void (Wire &)> f) const
{
	bottom_anchors_.applyToWires(f);
	bottom_.applyToWires(f);
	lower_cross_.applyToWires(f);

	for (auto &player : layers_) {
		player -> applyToWires(f);
	}

	top_anchors_.applyToWires(f);
}

Iterable<TreeModel::LayerIterator> TreeModel::layers() const
{
	UnwrapLayerPtr transform = [](const LayerPtr &lp) -> const Layer &{return *lp;};

	auto begin = boost::make_transform_iterator(layers_.cbegin(), transform);
	auto end = boost::make_transform_iterator(layers_.cend(), transform);

	return Iterable(begin, end);
}

void TreeModel::createShortcuts()
{
	for (auto &player : layers_)
		player -> createShortcuts();
}

bool TreeModel::isTopLayer(const ConstructionData &data)
{
	return data.upper_vertices.empty();
}

void TreeModel::nextLayer(spatial::ConstructionData &data)
{
	data.lower_vertices = std::move(data.upper_vertices);
	data.upper_vertices = {};
}

void TreeModel::processBottomIFaces(const BottomIFacesData &data)
{
	for (auto i : data.bottom_ifs)
	{
		processBottomIFace(BottomIFaceData(data, i));
	}
}

void TreeModel::processLooseVertices(const LooseVerticesData &data)
{
	std::set<VertexRef> loose_vertices;
	std::queue<VertexRef> queue;

	const Netlist::Graph &graph = data.graph;

	auto viters = graph.vertices();
	for (auto viter = viters.first; viter != viters.second; ++viter) {
		loose_vertices.insert(*viter);
	}

	for (VertexRef v : data.upper_vertices)
		queue.push(v);

	while (true) {
		while (!queue.empty()) {
			VertexRef connected = queue.front();
			queue.pop();

			auto nremoved = loose_vertices.erase(connected);
			assert (nremoved <= 1);

			if (!nremoved)
				continue;

			auto eiters = graph.outEdges(connected);
			for (auto eiter = eiters.first; eiter != eiters.second; ++eiter) {
				queue.push(graph.target(*eiter));
			}
		}

		if (loose_vertices.empty())
			break;


		// find top
		auto iv = loose_vertices.begin();

		for (; iv != loose_vertices.end(); ++iv) {
			if (isUpperIFace(data, *iv))
				break;
		}

		// no top, use any
		if (iv == loose_vertices.end())
			iv = loose_vertices.begin();

		// got to bottom
		std::set<VertexRef> subgraph;
		auto v = *iv;

		while (true) {
			auto result = subgraph.insert(v);

			if (!result.second)
				break;

			auto ie = graph.inEdges(v);
			if (ie.first == ie.second)
				break;

			v = graph.source(*ie.first);
		}

		Wire &wire = bottom_.make();
		data.upper_vertices.push_back(v);
		data.known_vertices.emplace(v, wire);

		queue.push(v);
	}
}

void TreeModel::processTopIFaces(const BottomIFacesData &data)
{
	for (auto i : data.top_ifs)
	{
		processTopIFace(TopIFaceData(data, i));
	}
}

void TreeModel::processLayer(const LayerData &data)
{
	MutableLayer &upper_layer = makeLayer();

	for (auto v : data.lower_vertices)
	{
		processVertex(VertexData(data, upper_layer, v));
	}
}

void TreeModel::processVertex(const VertexData &data)
{
	VertexRef vertex = data.vertex;
	const VertexInfo &vinfo = data.known_vertices.at(vertex);
	MutableLayer &upper_layer = data.upper_layer;

	NodeGroup *nodegroup = nullptr;
	Node *parent = vinfo.parent_node;

	if (parent) {
		nodegroup = parent -> child();
	}
	else {
		assert (upper_layer.index() == 0);
		nodegroup = bottomToGroup(vinfo.input);
	}

	if (!nodegroup) {
		Wire &manifold_cross = upper_layer.crossBelow().findOrMake(vinfo.input.connection_);
		nodegroup = &upper_layer.makeNodeGroup(vinfo.parent_node, manifold_cross);
	}

	assert (data.upper_layer.index() || vinfo.input.collection() == &bottom_);

	auto edge_iters = data.graph.outEdges(data.vertex);

	processEdges(EdgesData(data, *nodegroup, edge_iters.first, edge_iters.second));
}

void TreeModel::processEdges(const EdgesData &data)
{
	for (auto e = data.edges_begin; e != data.edges_end; ++e) {
		processEdge(EdgeData(data, *e));
	}
}

void TreeModel::processBottomIFace(const BottomIFaceData &data)
{
	auto iface = data.iface;

	Wire &wire = mkBottomInterfaceWire(iface);
	VertexRef v = data.nl_iface.at(iface);

	auto citer = data.known_vertices.find(v);
	if (citer != data.known_vertices.end()) {
		VertexInfo &vi = citer -> second;
		Wire &cross = lower_cross_.find(*vi.input.connection_);
		wire.connect(cross);
		return;
	}
		
	lower_cross_.make(wire.connection_);
	data.known_vertices.emplace(v, VertexInfo(wire));

	auto edge_iters = data.graph.outEdges(v);
	if (edge_iters.first == edge_iters.second) {
		//bottom pure output
		assert (iface -> direction() == Interface::Direction::OUT);
		return;
	}

	data.upper_vertices.push_back(v);
}


void TreeModel::processEdge(const EdgeData &data)
{
	EdgeRef edge = data.edge;
	VertexRef targetv = data.graph.target(edge);

	auto citer = data.known_vertices.find(targetv);

	if (citer != data.known_vertices.end()) {
		processLeaf(LeafData(data, citer -> second));
		return;
	}

	auto edge_iters = data.graph.outEdges(targetv);

	NodeGroup &nodegroup = data.nodegroup;
	Node &node = makeInverter(nodegroup, edge);
	data.known_vertices.emplace(targetv, node.output_);

	if (edge_iters.first == edge_iters.second) {
		assert (isUpperIFace(data, targetv));
		return;
	}

	data.upper_vertices.push_back(targetv);
	data.known_vertices.at(targetv).parent_node = &node;
}

void TreeModel::processLeaf(const LeafData &data)
{
	assert (data.nodegroup.getLayer() == &data.upper_layer);

	MutableLayer &layer = data.upper_layer;
	NodeGroup &ng = data.nodegroup;
	VertexInfo &kvi = data.known_vertex;
	const std::shared_ptr<Connection> &pconnection = kvi.input.connection_;

	//TODO: make adjacent passthrough downwards, if target wire is lower

	Node &inv = makeInverter(ng, data.edge);
	passThroughToTop(pconnection);
	layer.cross().findOrMake(pconnection);
	inv.output_.connect(pconnection);
}

bool TreeModel::isUpperIFace(const ConstructionData_AllReferences &data, VertexRef vertex)
{
	for (const ISingle *iface : data.top_ifs) {
		if (data.nl_iface.at(iface) == vertex)
			return true;
	}

	return false;
}

void TreeModel::processTopIFace(const TopIFaceData &data)
{
	const ISingle *iface = data.iface;

	VertexRef v = data.nl_iface.at(iface);
	const auto &known_vertices = data.known_vertices;

	VertexInfo vi = known_vertices.at(v);
	SingleWire &top = passThroughToTop(vi.input.connection_);

	Layer &layer = *layers_.back();
	layer.cross().findOrMake(top.connection_);

	useAsTopInterfaceWire(top, iface);
}

MutableLayer &TreeModel::makeLayer()
{
	layers_.emplace_back(std::make_unique<MutableLayer>(*this, layers_.size()));
	return *layers_.back();
}

void TreeModel::useAsInterfaceWire(Wire& wire, const ISingle *iface, Wires &anchors)
{
	anchors.make(wire.connection_);
	interface_[iface] = &wire;
}

Wire &TreeModel::mkBottomInterfaceWire(const ISingle *iface)
{
	Wire &wire = bottom_.make();
	useAsInterfaceWire(wire, iface, bottom_anchors_);
	return wire;
}

void TreeModel::useAsTopInterfaceWire(SingleWire &wire, const ISingle *iface)
{
	useAsInterfaceWire(wire, iface, top_anchors_);
	wire.addCrosser(top_anchors_);
}

bool TreeModel::isInterfaceWire(const SingleWire &wire, const Wires &anchors) const
{
	CollectedWire *anchor = anchors.findMax1(*wire.connection_);

	if (!anchor)
		return false;

	const auto &crossers = wire.getCrossers();

	return (crossers.find(&anchor -> asCrosser()) != crossers.end());

}

bool TreeModel::isInterfaceWire(const SingleWire &wire) const
{
	return isTopInterfaceWire(wire) || isBottomInterfaceWire(wire);
}

bool TreeModel::isTopInterfaceWire(const SingleWire &wire) const
{
	return isInterfaceWire(wire, top_anchors_);
}

bool TreeModel::isBottomInterfaceWire(const SingleWire &wire) const
{
	return isInterfaceWire(wire, bottom_anchors_);
}

Node &TreeModel::makeInverter(NodeGroup &ng, EdgeRef edge)
{
	Node &node = ng.makeInverter();
	nodeMap_[edge] = &node;
	return node;
}

SingleWire &TreeModel::passThroughToTop(const std::shared_ptr<Connection> &connection)
{
	Wire &start = findNearestOutput(*connection);
	return passThroughToTop(start);
}

SingleWire &TreeModel::passThroughToTop(Wire &input)
{
	Node *node;

	if (input.getLayer() == layers_.back().get()) {
		node = input.getNode();
		assert (node);
		assert (&node -> output_ == &input);
		return node -> output_;
	}

	NodeGroup *startGroup = inputToGroup(input);

	if (!startGroup) {
		node = input.getNode();

		if (!node) {
			assert (input.collection() == &bottom_);
			Wire &manifold = lower_cross_.findOrMake(input.connection_);
			NodeGroup &ng = layers_[0] -> insertNodeGroup(nullptr, manifold, 0);
			node = &ng.makePassThrough();
		}
	}
	else {
		node = &startGroup -> insertPassThrough(0);
	}

	unsigned int maxHeight = layers_.back() -> index();
	unsigned int height = node -> node_group_.layer_.asContainer().index();

	while (height < maxHeight) {
		MutableLayer &layer = *layers_[++height];
		node = &layer.chainPassThrough(*node, node -> output_);
	}

	return node -> output_;
}

Wire& TreeModel::findNearestOutput(const Connection &connection)
{
	Wire *result = nullptr;
	auto iplayer = layers_.rbegin();

	for (; iplayer != layers_.rend(); ++iplayer) {
		Layer &layer = **iplayer;
		auto *outputs = connection.layerOutputs(layer);

		if (!outputs)
			continue;

		for (Wire *output : *outputs) {
			if (output -> isConnected(connection))
				result = output;
		}

		if (result)
			return *result;
	}

	auto ipwire = bottom_.rbegin();

	for (; ipwire != bottom_.rend(); ++ipwire) {
		Wire &wire = **ipwire;

		if (wire.isConnected(connection))
			return wire;
	}

	assert (0);
	return *result;
}

NodeGroup *TreeModel::inputToGroup(const Wire &output)
{
	const Node *parent = output.getNode();

	if (!parent)
		return bottomToGroup(output);

	NodeGroup *child = parent -> child();

	if (!child)
		return nullptr;

	return child;
}

NodeGroup *TreeModel::bottomToGroup(const Wire &bottomWire)
{
	assert (std::any_of(bottom_.begin(), bottom_.end(), [&](auto &pwire){return pwire.get() == &bottomWire;}));

	for (auto &png : layers_[0] -> nodegroups()) {
		Wire &bottom_input = bottom_.find(*png -> manifold_cross_.connection_);

		if (&bottom_input == &bottomWire)
			return png.get();
	}

	return nullptr;
}

void TreeModel::computeVertical()
{
	blocks::index_t pos = 0;

	bottom_anchors_.computePositions();
	bottom_anchors_.setPosition(0);

	lower_cross_.setPosition(0);

	if (!lower_cross_.noNonAnchors())
		pos = lower_cross_.maxRelPos() + 1;

	for (auto &player : layers_) {
		if (player -> needsExtraBlockAtBottom())
			++pos;

		player -> setPosition(pos);
		player -> computeVertical();
		pos += player -> height();
	}

	top_anchors_.computePositions();
	top_anchors_.setPosition(pos - 1);

	height_ = pos;

	applyToWires([](Wire &w){if (!w.vertical()) w.computeAbsolutePosition();});
	applyToWires([](Wire &w){if (w.vertical()) w.computeExtents();});
}

void TreeModel::forLooseInputs(std::function<void (Wire &)> func)
{
	for (auto &pwire : bottom_) {
		const auto &l0groups = layers_[0] -> nodegroups();
		auto ipng = l0groups.begin();
		for (; ipng != l0groups.end(); ++ipng) {
			NodeGroup &ng = **ipng;
			if (ng.manifold_cross_.isConnected(*pwire))
				break;
		}

		if (ipng == l0groups.end())
			func(*pwire);
	}
}

MutableLayer::NodesIterable TreeModel::unwrap(const TreeModel::LayerPtr &pl)
{
	return pl -> nodes();
}

TreeModel::LayerNodesIterator TreeModel::nodes_begin() const
{
	return boost::make_transform_iterator(layers_.cbegin(), unwrap);
}

TreeModel::LayerNodesIterator TreeModel::nodes_end() const
{
	return boost::make_transform_iterator(layers_.cend(), unwrap);
}

TreeModel::NodesIterable TreeModel::nodes() const
{
	return CatGenerator<LayerNodesIterator>(nodes_begin(), nodes_end());
}

void TreeModel::assessReached(
		WorkingAndBrokenLinks &links, const Connector &startConnector,
		const std::map<const Connector *, bool> &wasReached) const
{
	Links &workingLinks = links.first;
	Links &brokenLinks = links.second;

	for (const auto &kv : wasReached) {
		const Connector *endConnector = kv.first;

		if (kv.second) {
			//std::cerr << "  ---> " << *kv.first << std::endl;
			workingLinks.emplace_back(&startConnector, endConnector);
		}
		else {
			//std::cerr << "  -X-> " << *kv.first << std::endl;
			brokenLinks.emplace_back(&startConnector, endConnector);
		}
	}
}

void TreeModel::assessLinks(
		const Connection &connection,
		std::map<const Connector *, bool> &wasReached, BlocksSim &sim) const
{
	for (const Wire *endWire : connection.inverterInputs()) {
		Node *endNode = endWire -> getNode();
		assert(endNode && endNode -> invert_);
		auto endTorchPositions = endNode -> getTorchPositions();

		for (blocks::Vec torchPos : endTorchPositions)
			wasReached[&getInputConnector(*endNode)] |= sim.torchWasReached(torchPos);
	}

	for (const auto &kv : interface_) {
		const ISingle *iface = kv.first;

		if (iface -> direction() != Interface::Direction::OUT)
			continue;

		const Wire *ifaceWire = kv.second;

		if (!ifaceWire -> isConnected(connection))
			continue;

		wasReached[&getIFaceConnector(*ifaceWire)] |= sim.get(iface);
	}
}

static bool isForwardNodeWire(const Wire &w)
{
	const auto *node = w.getNode();

	if (!node)
		return false;

	return !node -> backwards();
}

Links TreeModel::getLinks(const Connection &connection)
{
	Links result;
	std::unordered_set<const Connector *> sources, destinations;

	for (auto *source : connection.inverterOutputs()) {
		assert (isForwardNodeWire(*source));
		auto ir = sources.emplace(&source -> front());
		assert (ir.second);
	}

	for (auto *destination : connection.inverterInputs()) {
		assert (isForwardNodeWire(*destination));
		auto ir = destinations.emplace(&destination -> back());
		assert (ir.second);
	}

	/* TODO: Think about indexing interface wires in connection,
	 * we might also need them elsewhere.
	 */
	for (const auto &kv : interface_) {
		const auto &iface = *kv.first;
		const auto &wire = *kv.second;

		if (!wire.isConnected(connection))
			continue;

		switch (iface.direction()) {
		case SingleDirection::OUT: {
			auto ir = destinations.emplace(&wire.back());
			assert (ir.second);
			break;
		}
		case SingleDirection::IN: {
			auto ir = sources.emplace(&wire.front());
			assert (ir.second);
			break;
		}
		default:
			assert (0);
		}
	}

	result.reserve(sources.size() * destinations.size());

	for (const auto *src : sources) {
		for (const auto *dst : destinations) {
			result.emplace_back(src, dst);
		}
	}

	return result;
}

void TreeModel::assessLinks(
		const Connection &connection, WorkingAndBrokenLinks &links,
		const Connector &startConnector, BlocksSim &sim) const
{
	std::map<const Connector *, bool> wasReached;

	do {
		assessLinks(connection, wasReached, sim);
	}
	while (sim.extendTest());

	assessReached(links, startConnector, wasReached);
}

void TreeModel::createSegments()
{
	applyToWires([](Wire &w){w.createSegments();});
}


std::forward_list<VertexRef>
TreeModel::fixBrokenLinks(
		const std::map<const Connection *, netlist::VertexRef> &vertexMap)
{
	std::forward_list<VertexRef> result;

	bool fixed = false;

	for (auto &kv : vertexMap) {
		const auto &connection = *kv.first;
		auto vertex = kv.second;

		createSuperSegments(connection);

		switch (fixConnection(getLinks(connection))) {
		case FixConnectionResult::UNCHANGED:
			break;
		case FixConnectionResult::FIXED:
			fixed = true;
			break;
		case FixConnectionResult::BROKEN:
			result.push_front(vertex);
			break;
		default:
			assert (0);
		}
	}

	if (fixed && result.empty())
		breakTiming();

	return result;
}

ConnectionLinks TreeModel::assessLinks(const Blocks &blocks) const
{
	ConnectionLinks result;
	BlocksSim sim(blocks);

	for (const auto &pStartNode : nodes()) {
		const Node &startNode = *pStartNode;

		if (!startNode.invert_)
			continue;

		//std::cerr << "assess links from " << startNode << std::endl;
		sim.test(startNode.getTorchPositions());
		const Connection &connection = *startNode.output_.connection_;
		assessLinks(connection, result[&connection], getOutputConnector(startNode), sim);
	}

	for (const auto &kv : interface_) {
		const ISingle *iface = kv.first;

		if (iface -> direction() != Interface::Direction::IN)
			continue;

		const Wire &ifaceWire = *kv.second;

		//std::cerr << "assess links from " << *iface << std::endl;
		sim.test({blocks.ifaceMap().at(iface)});
		const Connection &connection = *ifaceWire.connection_;
		assessLinks(connection, result[&connection], getIFaceConnector(ifaceWire), sim);
	}

	return result;
}

}} //rhdl::spatial

