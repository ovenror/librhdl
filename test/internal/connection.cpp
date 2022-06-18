#include "entity/entity.h"

#include "representation/spatial/tree/connection.h"
#include "representation/netlist/netlist.h"
#include "representation/spatial/tree/mutablelayer.h"
#include "representation/spatial/tree/singlewire.h"
#include "representation/spatial/tree/treemodel.h"

#include <gtest/gtest.h>

using namespace rhdl::TM;

TEST(ConnectionTest, addNodeWire)
{
	rhdl::Entity dummy("DUMMY", {});
	rhdl::TreeModel model(dummy);
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
