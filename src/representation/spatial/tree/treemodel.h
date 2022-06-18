#ifndef TREEMODEL_H
#define TREEMODEL_H

#include "representation/representationbase.h"
#include "representation/netlist/netlist.h"
#include "representation/blocks/blocks.h"

#include "util/iterable.h"

#include <set>
#include <vector>
#include <stack>
#include <memory>
#include <functional>
#include "../tree/connection.h"
#include "../tree/container.h"
#include "../tree/divisivewires.h"
#include "../tree/fixoverlongwires.h"
#include "../tree/mutablelayer.h"

namespace rhdl {

class ISingle;

namespace netlist { class Netlist; }

namespace blocks {
class Blocks;
class BlocksSim;
}

namespace spatial {
class ConstructionData;
class IFaceData;
class VertexData;
class EdgesData;
class EdgeData;
class LeafData;

class ConstructionData_AllReferences;
using BottomIFacesData = ConstructionData_AllReferences;
using LooseVerticesData = ConstructionData_AllReferences;
using TopIFacesData = ConstructionData_AllReferences;
using LayerData = ConstructionData_AllReferences;
using BottomIFaceData = IFaceData;
using TopIFaceData = IFaceData;

class TreeModel : public RepresentationBase<TreeModel>, public Container
{
public:
	TreeModel(const netlist::Netlist &);

	TreeModel(
			const netlist::Netlist &source,
			const std::vector<const ISingle *> &lower,
			const std::vector<const ISingle *> &upper);

	TreeModel(const Entity &);

	const TreeModel &getModel() const override {return *this;}

	virtual ~TreeModel();

	//TreeModel &getModel() override {return *this;}

	void toBlocks(blocks::Blocks::Cuboid b) const;
	void toInterface(blocks::Blocks::Interface &interface) const;
	void computeSpatial();

	void createSegments();

	std::forward_list<netlist::VertexRef> fixBrokenLinks(
			const std::map<const Connection *, netlist::VertexRef> &vertexMap);
	Links getLinks(const Connection &);

	netlist::Netlist splitVertices(
			const std::forward_list<netlist::VertexRef> &vertices,
			const netlist::Netlist &source);

	ConnectionLinks assessLinks(const blocks::Blocks &blocks) const;
	bool hasBrokenLinks(const ConnectionLinks &assessment) const;

	void applyToWires(std::function<void(Wire &)> f) const;

	Wires &lowerCross() {return lower_cross_;}

	using LayerPtr = std::unique_ptr<MutableLayer>;
	using LayerContainer = std::vector<LayerPtr>;
	using UnwrapLayerPtr = std::function<const Layer &(const LayerPtr &lp)>;
	using LayerIterator = boost::transform_iterator<UnwrapLayerPtr, LayerContainer::const_iterator>;

	Iterable<LayerIterator> layers() const;
	const LayerContainer &mlayers() const {return layers_;}

	bool isTopInterfaceWire(const SingleWire &wire) const;
	bool isBottomInterfaceWire(const SingleWire &wire) const;
	bool isInterfaceWire(const SingleWire &wire, const Wires &anchors) const;
	bool isInterfaceWire(const SingleWire &wire) const;
	Node &makeInverter(NodeGroup &ng, netlist::EdgeRef edge);

protected:
	TreeModel(
			const Entity &entity, const Representation *parent,
			const Timing *timing);

	TreeModel(
			const Entity &entity, const Representation *parent,
			const Timing *timing,
			const std::vector<const ISingle *> &lower,
			const std::vector<const ISingle *> &upper);

	void createShortcuts();

	bool isTopLayer(const ConstructionData &data);
	void nextLayer(ConstructionData &data);

	void processBottomIFaces(const BottomIFacesData &data);
	void processLooseVertices(const LooseVerticesData &data);
	void processTopIFaces(const BottomIFacesData &data);

	void processBottomIFace(const BottomIFaceData &data);
	void processTopIFace(const TopIFaceData &data);

	void processLayer(const LayerData &data);
	void processVertex(const VertexData &data);
	void processEdges(const EdgesData &data);
	void processEdge(const EdgeData &data);
	void processLeaf(const LeafData &data);

	static bool isUpperIFace(
			const ConstructionData_AllReferences &data,
			netlist::VertexRef vertex);

	MutableLayer &makeLayer();

	Wire &mkBottomInterfaceWire(const ISingle *iface);
	void useAsTopInterfaceWire(SingleWire &wire, const ISingle *iface);
	void useAsInterfaceWire(Wire &wire, const ISingle *iface, Wires &anchors);

	SingleWire &passThroughToTop(const std::shared_ptr<Connection> &connection);
	SingleWire &passThroughToTop(Wire &input);
	Wire &findNearestOutput(const Connection &connection);
	NodeGroup *inputToGroup(const Wire &output);
	NodeGroup *bottomToGroup(const Wire &bottomWire);

	std::map<const Connection *, netlist::VertexRef> createModel(
			const netlist::Netlist &netlist,
			const std::vector<const ISingle *> &lower,
			const std::vector<const ISingle *> &upper);

	void preComputeSpatial();
	void placeBottomWires();
	void placeNodes();
	void computeWidth();

	void computeHorizontalCollectedWiresPosition();
	void computeVertical();
	void forLooseInputs(std::function<void(Wire &)> func);

	void assessLinks(
			const Connection &connection, WorkingAndBrokenLinks &links,
			const Connector &startConnector, blocks::BlocksSim &sim) const;

	void assessLinks(const Connection &connection,
			std::map<const Connector *, bool> &wasReached,
			blocks::BlocksSim &sim) const;

	void assessReached(WorkingAndBrokenLinks &links,
			const Connector &startConnector,
			const std::map<const Connector *, bool> &wasReached) const;

	blocks::Blocks::index_t xpos() const override {return 0;}
	blocks::Blocks::index_t ypos() const override {return 0;}

	using UnwrapType = std::function<MutableLayer::NodesIterable (const LayerPtr &)>;

	using LayerNodesIterator = boost::transform_iterator<UnwrapType, LayerContainer::const_iterator>;
	using NodesIterable = CatGenerator<LayerNodesIterator>;

	static MutableLayer::NodesIterable unwrap(const TreeModel::LayerPtr &pl);

	LayerNodesIterator nodes_begin() const;
	LayerNodesIterator nodes_end() const;

	NodesIterable nodes() const;

	//std::vector<std::unique_ptr<TM::Connection> > connections_;
	Wires bottom_anchors_;
	Wires bottom_;
	DivisiveWires lower_cross_;
	LayerContainer layers_;
	Wires top_anchors_;
	std::map<const ISingle *, const Wire *> interface_;
	std::map<netlist::EdgeRef, const Node *> nodeMap_;
};

}} //rhdl::spatial

#endif // TREEMODEL_H
