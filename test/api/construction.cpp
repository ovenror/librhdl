#include <gtest/gtest.h>
#include <rhdl/rhdl.h>

using rhdl::Structure;
using rhdl::Component;

TEST(ApiConstructionTest,InstantiateStructure) {
	Structure test("Test");
	test.abort();
}

TEST(ApiConstructionTest,GetEntityInterface) {
	Structure test("Test2");
	auto iface = test["foo"];
	test.abort();
}

TEST(ApiConstructionTest,InstantiateInverter) {
	Component inv("Inverter");
}

TEST(ApiConstructionTest,ExposeInverter) {
	Structure test("Test3");
	Component inv("Inverter");
	test["in"] >> inv >> test["out"];
}

TEST(ApiConstructionTest,ConnectInverters) {
	Component inv0("Inverter");
	Component inv1("Inverter");
	inv0 >> inv1;
}

TEST(ApiConstructionTest,UnnamedExposure) {
	Structure test("Test4");
	Component inv("Inverter");

	inv >> test;
	test >> inv;
}

TEST(ApiConstructionTest,UnnamedComplexExposure) {
	Structure test("Test5");
	Component nand("NAND");

	test >> nand;
	nand >> test;
}
