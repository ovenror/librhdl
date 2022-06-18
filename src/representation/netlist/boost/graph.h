#ifndef _NETLIST_BOOST_GRAPH_H_
#define _NETLIST_BOOST_GRAPH_H_

#include <boost/graph/adjacency_list.hpp>
#include <vector>

//using namespace boost;


namespace rhdl::netlist {

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS> GraphRep;
typedef GraphRep::vertex_descriptor VertexRef;
typedef GraphRep::edge_descriptor EdgeRef;

class Graph_Impl_Boost {

public:
	typedef boost::graph_traits<GraphRep>::vertex_iterator VertexIterator;
	typedef boost::graph_traits<GraphRep>::edge_iterator EdgeIterator;
	typedef boost::graph_traits<GraphRep>::out_edge_iterator OutEdgeIterator;
	typedef boost::graph_traits<GraphRep>::in_edge_iterator InEdgeIterator;

	std::pair<VertexIterator, VertexIterator> vertices() const {
		return boost::vertices(rep_);
	}

	VertexRef addVertex()
	{
		return boost::add_vertex(rep_);
	}

	EdgeRef connect(VertexRef from, VertexRef to)
	{
		return boost::add_edge(from, to, rep_).first;
	}

	VertexRef source(EdgeRef e) const
	{
		return boost::source(e, rep_);
	}

	VertexRef target(EdgeRef e) const
	{
		return boost::target(e, rep_);
	}

	std::pair<OutEdgeIterator, OutEdgeIterator> outEdges(VertexRef v) const
	{
		return boost::out_edges(v, rep_);
	}

	std::pair<InEdgeIterator, InEdgeIterator> inEdges(VertexRef v) const
	{
		return boost::in_edges(v, rep_);
	}

	size_t countOut(VertexRef v)
	{
		//VertexIterator begin, end;
		//std::tie(begin, end) = boost::out_edges(v, rep_);
		//return std::count_if(begin, end, true);

		auto iters = boost::out_edges(v, rep_);
		return std::distance(iters.first, iters.second);
	}

	size_t countIn(VertexRef v)
	{
		//VertexIterator begin, end;
		//std::tie(begin, end) = boost::in_edges(v, rep_);
		//return std::count_if(begin, end, true);

		auto iters = boost::in_edges(v, rep_);
		return std::distance(iters.first, iters.second);
	}

	bool isomorphic(Graph_Impl_Boost &g) const;
	bool hasCycles() const;

	void createOneway(VertexRef from, VertexRef to);
	void clearOutEdges(VertexRef vertex);
	void eat(VertexRef eater, VertexRef eaten);
	std::map<VertexRef, VertexRef> removeVertex(VertexRef vertex);
	std::map<VertexRef, VertexRef> removeDisconnectedVertices();
	std::map<VertexRef, VertexRef> absorb(const Graph_Impl_Boost &other);

	std::map<VertexRef, bool> initialStateMap() const;
	void simStep(std::map<VertexRef, bool> &state) const;

	operator std::string() const;

private:
	friend std::ostream &operator<<(std::ostream &os, const Graph_Impl_Boost &graph);

	GraphRep rep_;
};

std::ostream &operator<<(std::ostream &os, const Graph_Impl_Boost &graph);

}

#endif // _NETLIST_BOOST_GRAPH_H_
