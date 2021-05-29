#include "simtester.h"
#include "simfuzzer.h"
#include "construction/library.h"
#include "entity/einverter.h"

#include <gtest/gtest.h>

#include <string>
#include <tuple>

using namespace rhdl;

static void testEntity(const char *name)
{
    const auto &e = defaultLib.at(name);
    SimFuzzer fuzzer(e);
    EXPECT_TRUE(fuzzer.run())
    	<< "Failure testing Entity " << e.name() << std::endl
	<< fuzzer.getMsgs();
}

TEST(SimulationTest,Inverter)
{
	testEntity("Inverter");
}

TEST(SimulationTest,ClockDiv2)
{
	testEntity("ClockDiv2");
}

TEST(SimulationTest,D_Flipflop)
{
	testEntity("D_Flipflop");
}

TEST(SimulationTest,NOR)
{
	testEntity("NOR");
}


