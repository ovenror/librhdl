#ifndef GRAPH_IMPL_H
#define GRAPH_IMPL_H

#include "boost/graph.h"

namespace rhdl::netlist {

class Graph_Impl : public Graph_Impl_Boost {
public:
	Graph_Impl();
};

}

#endif // GRAPH_H
