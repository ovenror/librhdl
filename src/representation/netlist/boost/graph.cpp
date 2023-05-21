#include "representation/netlist/boost/graph.h"

#include "util/iterable.h"

#include "interface/isingle.h"

#include <boost/graph/graphviz.hpp>
#include <boost/graph/isomorphism.hpp>

#include <sstream>



using namespace boost;

namespace rhdl::netlist {

bool Graph_Impl_Boost::isomorphic(const Graph_Impl_Boost &g) const
{
	return boost::isomorphism(this -> rep_, g.rep_);
}

struct cycle_detector : public dfs_visitor<>
{
  cycle_detector( bool& has_cycle)
	: _has_cycle(has_cycle) { }

  template <class Edge, class Graph>
  void back_edge(Edge, Graph&) {
	_has_cycle = true;
  }
protected:
  bool& _has_cycle;
};

bool Graph_Impl_Boost::hasCycles() const
{
	bool has_cycle = false;
	cycle_detector vis(has_cycle);
	depth_first_search(rep_, visitor(vis));
	return has_cycle;
}

void Graph_Impl_Boost::clearOutEdges(VertexRef vertex)
{
	clear_out_edges(vertex, rep_);
}

void Graph_Impl_Boost::eat(VertexRef eater, VertexRef eaten)
{
	auto out_iters = adjacent_vertices(eaten, rep_);
	auto in_iters = inv_adjacent_vertices(eaten, rep_);

	for (auto iter = out_iters.first; iter != out_iters.second; ++iter) {
		connect(eater, *iter);
	}

	for (auto iter = in_iters.first; iter != in_iters.second; ++iter) {
		connect(*iter, eater);
	}

	rep_[eater] += std::move(rep_[eaten]);
	rep_[eaten] = {};

	clear_vertex(eaten, rep_);
}

static std::map<VertexRef, VertexRef> absorb_internal(
		GraphRep &dest, const GraphRep &src,
		std::function<bool(VertexRef)> filter = [](VertexRef r){std::ignore=r;return true;})
{
	graph_traits<GraphRep>::vertex_iterator src_vi, src_vi_end;
	std::map<VertexRef, VertexRef> vmap;

	std::tie(src_vi, src_vi_end) = vertices(src);
	for (;src_vi != src_vi_end; ++src_vi)
	{
		if (!filter(*src_vi))
			continue;

		auto newV = add_vertex(dest);
		dest[newV].ifaces_in = src[*src_vi].ifaces_in;
		dest[newV].ifaces_out = src[*src_vi].ifaces_out;
		vmap[*src_vi] = newV;
	}

	graph_traits<GraphRep>::edge_iterator src_ei, src_ei_end;

	std::tie(src_ei, src_ei_end) = edges(src);
	for (;src_ei != src_ei_end; ++src_ei) {
		VertexRef vfrom = source(*src_ei, src);
		VertexRef vto = target(*src_ei, src);

		if (!filter(vfrom) || !filter(vto))
			continue;

		add_edge(vmap.at(vfrom), vmap.at(vto), dest);
	}

	return vmap;
}

std::map<VertexRef, VertexRef> Graph_Impl_Boost::removeDisconnectedVertices()
{
	GraphRep next;
	auto map = absorb_internal(
			next, rep_, [&](VertexRef v){
				return countIn(v) || countOut(v) ||
						!rep_[v].ifaces_in.empty() ||
						!rep_[v].ifaces_out.empty();
			});
	rep_ = next;
	return map;
}

std::map<VertexRef, VertexRef> Graph_Impl_Boost::removeVertex(VertexRef vertex)
{
	GraphRep next;
	auto map = absorb_internal(next, rep_, [&](VertexRef v){return v != vertex;});
	rep_ = next;
	return map;
}

std::map<VertexRef, VertexRef> Graph_Impl_Boost::absorb(const Graph_Impl_Boost &other)
{
	return absorb_internal(rep_, other.rep_);
}

std::map<VertexRef, bool> Graph_Impl_Boost::initialStateMap() const
{
	VertexIterator begin, end;
	std::map<VertexRef, bool> result;

	std::tie(begin, end) = vertices();
	for (auto iter = begin; iter != end; ++iter) {
		result[*iter] = false;
	}

	return result;
}

void Graph_Impl_Boost::simStep(std::map<VertexRef, bool> &state) const
{
	std::map<VertexRef, bool> next_state;

	VertexIterator vin_begin, vin_end;
	std::tie(vin_begin, vin_end) = vertices();
	for (auto vin_iter = vin_begin; vin_iter != vin_end; ++vin_iter) {
		auto vout_iters = inv_adjacent_vertices(*vin_iter, rep_);
		auto vout_begin = vout_iters.first;
		auto vout_end = vout_iters.second;

		next_state[*vin_iter] = false;

		for (auto vout_iter = vout_begin; vout_iter != vout_end; ++vout_iter) {
			if (state[*vout_iter] == false) {
				next_state[*vin_iter] = true;
				break;
			}
		}
	}

	state = next_state;
}

Graph_Impl_Boost::operator std::string() const
{
	std::stringstream ss;
	ss << *this;
	return ss.str();
}

class VertexWriter {
public:
	VertexWriter(const GraphRep &rep) : rep_(rep) {}

	template <class VertexOrEdge>
	void operator()(std::ostream& out, const VertexOrEdge& v) const
	{
		if (!rep_[v].ifaces_in.empty() || !rep_[v].ifaces_out.empty()) {
			out << "[shape=doublecircle, label=\"" << v;

			for (auto iface : rep_[v].ifaces_in) {
				out << "\n<" << iface -> name();
			}

			for (auto iface : rep_[v].ifaces_out) {
				out << "\n" << iface -> name() << ">";
			}

			out << "\"]";
		}
	}

private:
	const GraphRep &rep_;
};


std::ostream &operator<<(std::ostream &os, const Graph_Impl_Boost &graph)
{
	write_graphviz(os, graph.rep_, VertexWriter(graph.rep_));
	return os;
}

void Graph_Impl_Boost::createOneway(VertexRef from, VertexRef to)
{
	auto middle = addVertex();
	connect(from, middle);
	connect(middle, to);
}

bool Graph_Impl_Boost::isInternal(VertexRef v) const
{
	return countOut(v) && countIn(v);
}

void Graph_Impl_Boost::clear(VertexRef vertex)
{
	clear_vertex(vertex, rep_);
}

void Graph_Impl_Boost::clear_out(VertexRef vertex)
{
	clear_out_edges(vertex, rep_);
}

void Graph_Impl_Boost::clear_in(VertexRef vertex)
{
	clear_in_edges(vertex, rep_);
}

}
