#include "transformation/bgtree/connection.h"
#include "transformation/bgtree/singlewire.h"
#include "transformation/bgtree/treemodel.h"
#include "transformation/bgtree/mutablelayer.h"

#include "entity/entity.h"
#include "representation/netlist/netlist.h"

#include <gtest/gtest.h>

using namespace rhdl::TM;

TEST(ConnectionTest, addNodeWire)
{
	rhdl::Entity dummy("DUMMY");
	std::map<const Connection *, rhdl::VertexRef> dummyMap;
	rhdl::TreeModel model(rhdl::Netlist(dummy, nullptr, nullptr), {}, {}, dummyMap);
	SingleWire first(model, false);
	MutableLayer layer(model, 0);
	Wire &manifold(layer.cross().make());
	NodeGroup nodeGroup(layer, 0, nullptr, manifold);
	Node node(nodeGroup, 0, manifold, true);

	EXPECT_EQ(manifold.connection_ -> inverterInputs().size(), 1UL);
	EXPECT_EQ(manifold.connection_ -> inverterInputs()[0], &node.input_);

	Node node2(nodeGroup, 1, manifold, true);

	EXPECT_EQ(manifold.connection_ -> inverterInputs().size(), 2UL);
}
