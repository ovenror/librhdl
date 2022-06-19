#ifndef NETLIST_H
#define NETLIST_H

#include "graph_impl.h"
#include <forward_list>
#include "../mappedrepresentation.h"

namespace rhdl {

class ISingle;
class Simulator;

namespace netlist {

class Netlist : public MappedRepresentation<Netlist, VertexRef>
{
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

	virtual ~Netlist() {}

	const Graph &graph() const {return graph_;}

	virtual std::unique_ptr<Simulator> makeSimulator(bool use_behavior) const override;    
	static std::string InterfaceToString(const InterfaceMap &);

	bool isOpen(VertexRef vertex) const {return graph_.isInternal(vertex);}

	void splitVertex(VertexRef vertex);
	void removeVertex(VertexRef vertex);

	bool hasCycles() const {return graph_.hasCycles();}

	InterfaceMap copyInto(Graph &target) const;
	void removeDisconnectedVertices();
	void remapInterface(const std::map<VertexRef, VertexRef> &vertexMap);

private:
	Graph graph_;
};

}}

#endif // NETLIST_H
