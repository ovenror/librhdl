#include "simtester.h"
#include "simprinter.h"

#include "representation/structural/structure.h"

#include "simulation/netlistsim.h"
#include "simulation/blockssim.h"

#include "entity/entity.h"
#include "entity/timing.h"

#include "interface/isingle.h"

#include <string>
#include <sstream>

using namespace rhdl;

SimTester::SimTester(const Entity &e)
{
    ifaces_ = e.interface().flat();

    for (const ISingle *iface : ifaces_)
    {
        switch (iface -> direction()) {
        case Interface::Direction::IN:
            if (iface -> name() == "clk") {
                assert (!iClock_);
                iClock_ = iface;
                break;
            }
            if (iface -> name() == "reset") {
                assert (!iReset_);
                iReset_ = iface;
                break;
            }
            inIFaces_.push_back(iface);
            break;
        case Interface::Direction::OUT:
            outIFaces_.push_back(iface);
            break;
        }
    }

    for (Representation::TypeID type : Representations::validTypeIDs)
        e.getRepresentation(type);

    for (const Representation &r : e.representations())
        addSimulator(r);
}


void SimTester::addSimulator(const Representation &r)
{
    if (r.typeID() == structural::Structure::ID) {
        addSimulator(r, true);
    }

    addSimulator(r, false);
}

void SimTester::addSimulator(const Representation &r, bool use_behavior)
{
    auto ptr = r.makeSimulator(use_behavior);

    if (ptr == nullptr)
    	return;

    ptr = std::make_unique<SimPrinter>(std::move(ptr), ifaces_);
    sims_[r.timing()].push_back(std::move(ptr));
}

bool SimTester::loop(unsigned int num_steps)
{    
    for (unsigned int nstep=0; nstep < num_steps; ++nstep) {
        //std::cerr << "  executing step #" << nstep << std::endl;
        if (!processInput())
            return false;

        if (!internalStep())
            return false;
    }

    return true;
}

bool SimTester::correctWithinTiming()
{
    for (auto *iface : outIFaces_) {
        assert (iface -> direction() == Interface::Direction::OUT);
        if (!correctWithinTiming(iface))
            return false;
    }

    return true;
}

bool SimTester::correctBetweenTimings()
{
    for (auto *iface : outIFaces_) {
        assert (iface -> direction() == Interface::Direction::OUT);
        if (!correctBetweenTimings(iface))
            return false;
    }

    return true;
}

bool SimTester::correctWithinTiming(const ISingle *iface)
{
    bool error = false;
    std::stringstream timings_err;

    for (auto &kv : sims_) {
        std::stringstream timing_err;
        bool timing_error = false;
        const Timing *timing = kv.first;
        auto &timedSims = kv.second;

        auto isim = timedSims.begin();
        bool reference = (*isim ) -> get(iface);
        std::string refName = typeid(**isim ).name();
        ++isim;

        for (; isim  < timedSims.end(); ++isim ) {
            if ((*isim ) -> get(iface) != reference) {
                timing_err << "reference: " << refName << std::endl;
                timing_err << "deviant: " << typeid(**isim ).name() << std::endl;

                timing_error = true;
            }
        }

        if (!timing_error)
            continue;

        error = true;

        std::stringstream timing_hdr;
        timing_hdr << "timing:" << (void *) timing << std::endl;

        timings_err << timing_hdr.str() << timing_err.str();
    }

    if (!error)
        return true;

    std::stringstream hdr;
    hdr << "interface:" << (std::string) *iface << std::endl;
    msgs_ += hdr.str() + timings_err.str();

    std::cerr << msgs_;

    return false;
}

bool SimTester::correctBetweenTimings(const ISingle *iface)
{
    bool error = false;
    std::stringstream err;

    auto itiming = sims_.begin();
    const Timing *refTiming = itiming -> first;
    assert (!itiming -> second.empty());
    Simulator &refSim = *itiming -> second.front();
    bool reference = refSim.get(iface);
    ++itiming;

    for (; itiming != sims_.end(); ++itiming) {
        assert (!itiming -> second.empty());
        Simulator &sample = *itiming -> second.front();

        if (sample.get(iface) != reference) {
            err << "reference timing: " << (void *) refTiming << std::endl;
            err << "deviant timing: " << (void *) itiming -> first << std::endl;

            error = true;
        }
    }

    if (!error)
        return true;

    std::stringstream hdr;
    hdr << "interface:" << (std::string) *iface << std::endl;
    msgs_ += hdr.str() + err.str();

    std::cerr << msgs_;

    return false;
}

bool SimTester::processInput()
{
    //debug_prolog("INPUT");
    processInput_internal();
    //debug_epilog("INPUT");

    return testWithMsg("input processing");
}

bool SimTester::internalStep()
{
    //debug_prolog("STEP");
    internalStep_internal();
    //debug_epilog("STEP");

    return testWithMsg("simulator step");
}

bool SimTester::stepAfterInput()
{
    debug_prolog("INPUT AND STEP");
    stepAfterInput_internal();
    debug_epilog("INPUT AND STEP");

    return testWithMsg("input and step");
}

bool SimTester::testWithMsg(std::string msg)
{
    if (correctWithinTiming())
        return true;

    msgs_ = msg + "\n" + msgs_;
    return false;
}

void SimTester::internalStep_internal()
{
    for (auto &kv : sims_)
        for (auto &sim : kv.second)
            sim -> internalStep();
}

void SimTester::stepAfterInput_internal()
{
    for (auto &kv : sims_)
        for (auto &sim : kv.second)
            sim -> step();
}

void SimTester::processInput_internal()
{
    for (auto &kv : sims_)
        for (auto &sim : kv.second)
            sim -> processInput();
}

void SimTester::debug_prolog(std::string what)
{
    std::cerr << std::endl << std::endl << "##################" << std::endl << std::endl;
    std::cerr << "BEFORE " << what << std::endl;

    debug_common();

    std::cerr << std::endl << "  ... stepping ... " << std::endl << std::endl;
}

void SimTester::debug_epilog(std::string what)
{
    std::cerr << "AFTER " << what << std::endl << std::endl;

    debug_common();

    std::cerr << std::endl << "##################" << std::endl << std::endl;
}

void SimTester::debug_common()
{
    for (auto &kv : sims_) {
        std::cerr << "Timing: " << (void*) kv.first << std::endl;

        for (auto &sim : kv.second)
            std::cerr << *sim;
    }
}

void SimTester::set(const ISingle *iface, bool value)
{
    assert (iface -> direction() == Interface::Direction::IN);

    for (auto &kv : sims_) {
        for (auto &sim : kv.second) {
            sim -> set(iface, value);
        }
    }
}

bool SimTester::get(const ISingle *iface)
{
    assert (correctBetweenTimings());

    return (*sims_.begin() -> second.begin()) -> get(iface);
}

#if 0
bool SimTester::get(const ISingle *iface) const
{
    bool val = (*sims_.begin())[0] -> get(iface);

    return val;
}
#endif

void SimTester::resetInputs()
{
    for (auto &kv : sims_)
        for (auto &sim : kv.second)
            sim -> resetInput();
}
