#include "simfuzzer.h"

#include "interface/isingle.h"

#include "entity/entity.h"

#include <iostream>

namespace rhdl {

SimFuzzer::SimFuzzer(const rhdl::Entity &e)
    : tester_(e), uniform_(0,1), entity_(e), clock_(tester_.hasClock()),
      clkState_(false)
{

}

std::string SimFuzzer::getMsgs()
{
    return tester_.getMsgs();
}

bool SimFuzzer::reset()
{
    bool hReset = tester_.hasReset();

    if (hReset)
        tester_.setReset(true);

    if (clock_) {
        settle();
        tester_.setClock(true);
    }

    settle();

    if (!tester_.correctBetweenTimings())
        return false;

    if (clock_)
        tester_.setClock(false);

    if (hReset)
        tester_.setReset(false);

    return true;
}

bool SimFuzzer::functionalTest()
{
    for (int cycle=0; cycle<50; ++cycle)
    {
            if (!settle())
                return false;

            if (!tester_.correctBetweenTimings())
                return false;

            if (clock_ && cycle&1) {
                clkEdge();

                if (clkState_)
                    continue;
            }

            flip();
    }

    return true;
}

bool SimFuzzer::stressTest()
{
    for (int cycle=0; cycle<50; ++cycle)
    {
            if (!tester_.loop(1 + uniform_(randgen_) + uniform_(randgen_)))
                return false;

            flip();
    }

    return true;
}

bool SimFuzzer::run()
{
    if (!reset())
        return false;

    if (!functionalTest())
        return false;

    if (!stressTest())
        return false;

    return true;
}

void SimFuzzer::flip()
{
    //std::cerr << "flippin..." << std::endl;

    for (auto iface : tester_.inIFaces()) {
        //std::cerr << "  " << iface -> name() << " (" << iface << "): ";
        if (uniform_(randgen_)) {
            //std::cerr << "1";
            tester_.set(iface, true);
        }
        else {
            //std::cerr << "0";
            tester_.set(iface, false);
        }
        //std::cerr << std::endl;
    }
}

bool SimFuzzer::settle()
{
    return tester_.loop(tSettle_);
}

}

void SimFuzzer::clkEdge()
{
    assert (clock_);
    clkState_ = !clkState_;
    tester_.setClock(clkState_);
}

