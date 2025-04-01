#ifndef NETLIST_H
#define NETLIST_H

#include "../mappedrepresentation.h"
#include "graph_impl.h"
#include "interface/direction.h"
#include <forward_list>

namespace rhdl {

class ISingle;
class Simulator;

namespace netlist {

class Netlist : public MappedRepresentation<Netlist, VertexRef>
{
	static_assert(ID == RHDL_NETLIST);

public:
	typedef Graph_Impl Graph;

	Netlist(
			const Entity &entity,
			Graph graph, InterfaceMap ifaceMap,
			const Representation *parent = nullptr,
			const Timing *timing = nullptr);

	Netlist(const Entity &entity, const Representation *parent,
			const Timing *timing);

	Netlist(const Netlist &, std::forward_list<VertexRef> toSplit);
	Netlist(Netlist &&) = default;

	virtual ~Netlist() {}

	const Graph &graph() const {return graph_;}

	virtual std::unique_ptr<Simulator> makeSimulator(bool use_behavior) const override;    
	static std::string InterfaceToString(const InterfaceMap &);

	bool isOpen(VertexRef vertex) const {return graph_.isInternal(vertex);}

	void horizontalSplitVertices(std::forward_list<VertexRef> toSplit);
	void horizontalSplitVertex(VertexRef vertex);
	void verticalSplitVertices(std::forward_list<VertexRef> toSplit);
	void verticalSplitVertex(VertexRef vertex);

	void removeVertex(VertexRef vertex);

	bool hasCycles() const {return graph_.hasCycles();}

	InterfaceMap copyInto(Graph &target) const;
	void removeDisconnectedVertices();
	void remapInterface(const std::map<VertexRef, VertexRef> &vertexMap);

	virtual void compute_content(std::string&) const override;

private:
	static Netlist make(
			const Entity &entity,
			Graph graph, InterfaceMap ifaceMap,
			const Representation *parent = nullptr,
			const Timing *timing = nullptr);

	void removeUnnecessaryOneways();

	size_t iCountIn(VertexRef) const;
	size_t iCountOut(VertexRef) const;

	bool iHasIn(VertexRef) const;
	bool iHasOut(VertexRef) const;

	void initIFaceProperties();

	void eat(VertexRef, VertexRef);

	bool existsElementRef(VertexRef v) override {return v < graph_.size();}

	void dot(std::string extra = "") const;

	Graph graph_;
};

}}

#endif // NETLIST_H
