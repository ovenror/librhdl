#ifndef TREEMODEL_H
#define TREEMODEL_H

#include "container.h"
#include "connection.h"
#include "mutablelayer.h"
#include "divisivewires.h"
#include "fixoverlongwires.h"

#include "representation/netlist/netlist.h"
#include "representation/blocks/blocks.h"

#include "util/iterable.h"

#include <set>
#include <vector>
#include <stack>
#include <memory>
#include <functional>

namespace rhdl {

class ISingle;

namespace netlist { class Netlist; }

namespace blocks {
class Blocks;
class BlocksSim;
}

namespace TM {
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
}

class TreeModel : public TM::Container
{
public:
	TreeModel(const netlist::Netlist &netlist, const std::vector<const ISingle *> &lower,
			const std::vector<const ISingle *> &upper,
			std::map<const TM::Connection *, netlist::VertexRef> &vertexMap);

	const TreeModel &getModel() const override {return *this;}
	//TreeModel &getModel() override {return *this;}

	void toBlocks(blocks::Blocks::Cuboid b);
	void toInterface(blocks::Blocks::Interface &interface);
	void computeSpatial();

	void createSegments();
	TM::ConnectionLinks assessLinks(const blocks::Blocks &blocks) const;
	bool hasBrokenLinks(const TM::ConnectionLinks &assessment);
	std::forward_list<const TM::Connection *> fixBrokenLinks(
			const TM::ConnectionLinks &assessment, blocks::Blocks &b);

	netlist::Netlist splitConnections(
			std::forward_list<const TM::Connection *> connections,
			std::map<const TM::Connection *, netlist::VertexRef> vertexMap,
			const netlist::Netlist &source);

	//bool hasBrokenLinks(const Blocks &b);

	void applyToWires(std::function<void(TM::Wire &)> f);

	TM::Wires &lowerCross() {return lower_cross_;}

	using LayerPtr = std::unique_ptr<TM::MutableLayer>;
	using LayerContainer = std::vector<LayerPtr>;
	using UnwrapLayerPtr = std::function<const TM::Layer &(const LayerPtr &lp)>;
	using LayerIterator = boost::transform_iterator<UnwrapLayerPtr, LayerContainer::const_iterator>;

	Iterable<LayerIterator> layers() const;
	const LayerContainer &mlayers() const {return layers_;}

	bool isTopInterfaceWire(const TM::SingleWire &wire) const;
	bool isBottomInterfaceWire(const TM::SingleWire &wire) const;
	bool isInterfaceWire(const TM::SingleWire &wire, const TM::Wires &anchors) const;
	bool isInterfaceWire(const TM::SingleWire &wire) const;
	TM::Node &makeInverter(TM::NodeGroup &ng, netlist::EdgeRef edge);

protected:
	void createShortcuts();

	bool isTopLayer(const TM::ConstructionData &data);
	void nextLayer(TM::ConstructionData &data);

	void processBottomIFaces(const TM::BottomIFacesData &data);
	void processLooseVertices(const TM::LooseVerticesData &data);
	void processTopIFaces(const TM::BottomIFacesData &data);

	void processBottomIFace(const TM::BottomIFaceData &data);
	void processTopIFace(const TM::TopIFaceData &data);

	void processLayer(const TM::LayerData &data);
	void processVertex(const TM::VertexData &data);
	void processEdges(const TM::EdgesData &data);
	void processEdge(const TM::EdgeData &data);
	void processLeaf(const TM::LeafData &data);

	static bool isUpperIFace(
			const TM::ConstructionData_AllReferences &data,
			netlist::VertexRef vertex);

	TM::MutableLayer &makeLayer();

	TM::Wire &mkBottomInterfaceWire(const ISingle *iface);
	void useAsTopInterfaceWire(TM::SingleWire &wire, const ISingle *iface);
	void useAsInterfaceWire(TM::Wire &wire, const ISingle *iface, TM::Wires &anchors);

	TM::SingleWire &passThroughToTop(const std::shared_ptr<TM::Connection> &connection);
	TM::SingleWire &passThroughToTop(TM::Wire &input);
	TM::Wire &findNearestOutput(const TM::Connection &connection);
	TM::NodeGroup *inputToGroup(const TM::Wire &output);
	TM::NodeGroup *bottomToGroup(const TM::Wire &bottomWire);

	void preComputeSpatial();
	void placeBottomWires();
	void placeNodes();
	void computeWidth();

	void computeHorizontalCollectedWiresPosition();
	void computeVertical();
	void forLooseInputs(std::function<void(TM::Wire &)> func);

	void assessLinks(
			const TM::Connection &connection, TM::WorkingAndBrokenLinks &links,
			const TM::Connector &startConnector, blocks::BlocksSim &sim) const;

	void assessLinks(const TM::Connection &connection,
			std::map<const TM::Connector *, bool> &wasReached,
			blocks::BlocksSim &sim) const;

	void assessReached(TM::WorkingAndBrokenLinks &links,
			const TM::Connector &startConnector,
			const std::map<const TM::Connector *, bool> &wasReached) const;

	blocks::Blocks::index_t xpos() const override {return 0;}
	blocks::Blocks::index_t ypos() const override {return 0;}

	using UnwrapType = std::function<TM::MutableLayer::NodesIterable (const LayerPtr &)>;

	using LayerNodesIterator = boost::transform_iterator<UnwrapType, LayerContainer::const_iterator>;
	using NodesIterable = CatGenerator<LayerNodesIterator>;

	static TM::MutableLayer::NodesIterable unwrap(const TreeModel::LayerPtr &pl);

	LayerNodesIterator nodes_begin() const;
	LayerNodesIterator nodes_end() const;

	NodesIterable nodes() const;

	//std::vector<std::unique_ptr<TM::Connection> > connections_;
	TM::Wires bottom_anchors_;
	TM::Wires bottom_;
	TM::DivisiveWires lower_cross_;
	LayerContainer layers_;
	TM::Wires top_anchors_;
	std::map<const ISingle *, const TM::Wire *> interface_;
	std::map<netlist::EdgeRef, const TM::Node *> nodeMap_;
};

}

#endif // TREEMODEL_H
