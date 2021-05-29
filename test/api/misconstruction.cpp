#include <gtest/gtest.h>
#include <rhdl/rhdl.h>

#include <functional>

using rhdl::Structure;
using rhdl::Component;
using rhdl::ConstructionException;
using rhdl::Errorcode;

static void test(Errorcode ec, std::function<void()> deed)
{
    try {
        deed();
        ADD_FAILURE() << "Expected ConstructionException with code " << (int) ec << ".";
    }
    catch (const ConstructionException &e) {
        EXPECT_EQ(e.errorcode(), ec) << "Unexpected exception: " << e.what();
    }
}

TEST(ApiMisconstruction, incompatibleDirections)
{
    Component inv0("Inverter");
    Component inv1("Inverter");

    test(Errorcode::E_INCOMPATIBLE_DIRECTIONS, [&](){
        inv0["out"] >> inv1["out"];
    });

    test(Errorcode::E_INCOMPATIBLE_DIRECTIONS, [&](){
        inv0["out"] << inv1["out"];
    });

    test(Errorcode::E_INCOMPATIBLE_DIRECTIONS, [&](){
        inv0["in"] >> inv1["in"];
    });

    test(Errorcode::E_INCOMPATIBLE_DIRECTIONS, [&](){
        inv0["in"] << inv1["in"];
    });

}

TEST(ApiMisconstruction, wrongOp)
{
    Component inv0("Inverter");
    Component inv1("Inverter");

    test(Errorcode::E_DIRECTION_OPPOSES_OPERATOR, [&](){
        inv0["in"] >> inv1["out"];
    });

    test(Errorcode::E_DIRECTION_OPPOSES_OPERATOR, [&](){
        inv0["out"] << inv1["in"];
    });

    inv0["in"] = inv1["out"];
    inv0["out"] = inv1["in"];
}

TEST(ApiMisconstruction, open2open)
{
    Component c0("RS_Latch");
    Component c1("RS_Latch");

    test(Errorcode::E_OPEN_TO_OPEN, [&](){
        c0["Q"] >> c1["S"];
    });
}

TEST(ApiMisconstruction, ambiguousConnection)
{
    Component nand("NAND");

    test(Errorcode::E_FOUND_MULTIPLE_COMPATIBLE_INTERFACES, [&](){
        nand >> nand;
    });
}

TEST(ApiMisconstruction, alreadyConnectedToOpen)
{
    Structure foo("MCTest", Structure::CREATE_STATEFUL);
    Component nor("NOR");

    nor >> nor["in0"];

    test(Errorcode::E_ALREADY_CONNECTED_TO_OPEN, [&](){
        nor >> foo;
    });

    test(Errorcode::E_ALREADY_CONNECTED_TO_OPEN, [&](){
        nor >> nor["in1"];
    });
}

TEST(ApiMisconstruction, statefulComponentInStatelessStructure)
{
    test(Errorcode::E_STATEFUL_COMPONENT_IN_STATELESS_ENTITY, []()
    {
        Structure foo("MCTest2");
        Component rs("RS_Latch");

        foo >> rs["S"];
    }
    );

    Structure foo("MCTest3");
    Component inv("Inverter");
    Component inv2("Inverter");
    Component rs("RS_Latch");

    inv["out"] >> rs["S"]; //ok, common net becomes stateful
    rs["Q"] >> inv2["in"]; //dto.

    test(Errorcode::E_STATEFUL_COMPONENT_IN_STATELESS_ENTITY, [&]()
    {
        foo["something"] >> inv["in"];
    }
    );
}

TEST(ApiMisconstruction, introduceCycle)
{
    test(Errorcode::E_NETLIST_CONTAINS_CYCLES, []() {
        Structure foo("ShouldNotContainCycles", Structure::CREATE_STATELESS);

        Component inv1("Inverter");
        Component inv2("Inverter");

        inv1 >> inv2;
        inv2 >> inv1;
        foo["something"] >> inv1;

        foo.finalize();
    });
}

TEST(ApiMisconstruction, noSuchEntity)
{
    test(Errorcode::E_NO_SUCH_ENTITY, []() {
        Component nope("fsdhfjkljshdfklasjshdfkljashkldfjh");
    });
}

TEST(ApiMisconstruction, entityExists)
{
    test(Errorcode::E_ENTITY_EXISTS, []() {
        Structure nope("Inverter");
    });
}

