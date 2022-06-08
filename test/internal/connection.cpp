#include "representation/spatial/bgtree/connection.h"
#include "representation/spatial/bgtree/singlewire.h"
#include "representation/spatial/bgtree/treemodel.h"
#include "representation/spatial/bgtree/mutablelayer.h"

#include "entity/entity.h"
#include "representation/netlist/netlist.h"

#include <gtest/gtest.h>

using namespace rhdl::TM;

TEST(ConnectionTest, addNodeWire)
{
	namespace nl = rhdl::netlist;

	rhdl::Entity dummy("DUMMY", {});
	rhdl::TreeModel model(nl::Netlist(dummy, nullptr, nullptr), {}, {});
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
