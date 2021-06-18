#ifndef NETLIST_H
#define NETLIST_H

#include "graph_impl.h"
#include "representation/representationbase.h"

namespace rhdl {

class ISingle;
class Simulator;

namespace netlist {

struct Netlist : public RepresentationBase<Netlist>
{
	typedef Graph_Impl Graph;
	typedef std::map <const ISingle *, VertexRef> Interface;

	Netlist(const Entity &entity, const Representation *parent,
			const Timing *timing);

	virtual std::unique_ptr<Simulator> makeSimulator(bool use_behavior) const override;    
	static std::string InterfaceToString(const Interface &nli);

	//TODO: transformation interface?
	Netlist::Interface copyInto(Netlist &target) const;
	void removeDisconnectedVertices();

	void createOneway(VertexRef from, VertexRef to);
	void splitVertex(VertexRef vertex);
	void removeVertex(VertexRef vertex);

	void remapInterface(const std::map<VertexRef, VertexRef> &vertexMap);

	bool ready_;
	Interface interface_;
	Graph graph_;
};

}}

#endif // NETLIST_H
