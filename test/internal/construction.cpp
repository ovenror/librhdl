#include <gtest/gtest.h>

#include "construction/library.h"

using namespace rhdl;

TEST(InternalConstructionTest,InstantiateStructure) {
	Ent test("Test");
}

TEST(InternalConstructionTest,GetEntityInterface) {
	Ent test("Test2");
	auto iface = test["foo"];
}

TEST(InternalConstructionTest,UnnamedExposure) {
	Ent test("Test3");
	Part inv("Inverter");

	inv >> test;
}

