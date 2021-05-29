#ifndef SIMTESTER_H
#define SIMTESTER_H

#include "simprinter.h"

#include <memory>
#include <vector>
#include <map>

namespace rhdl {
class Entity;
class ISingle;
class Timing;
class Representation;
}

using namespace rhdl;

class SimTester
{
public:
    const int num_steps = 100;
    SimTester(const rhdl::Entity &e);

    bool loop(unsigned int num_steps = 100);
    std::string getMsgs() {return msgs_;}

    void resetInputs();

    bool processInput();
    bool internalStep();
    bool stepAfterInput();

    bool testWithMsg(std::string msg);

    bool correctWithinTiming();
    bool correctBetweenTimings();
    bool correctWithinTiming(const ISingle *);
    bool correctBetweenTimings(const ISingle *iface);

    void set(const ISingle *, bool value = true);
    bool get(const ISingle *iface);

    bool hasClock() {return iClock_;}
    bool hasReset() {return iReset_;}

    void setClock(bool value) {set(iClock_, value);}
    void setReset(bool value) {set(iReset_, value);}

    const ISingle *iClock() {return iClock_;}
    const ISingle *iReset() {return iReset_;}

    const std::vector<const ISingle *> &ifaces() const {return ifaces_;}
    const std::vector<const ISingle *> &inIFaces() const {return inIFaces_;}
    const std::vector<const ISingle *> &outIFaces() const {return outIFaces_;}

private:
    void addSimulator(const Representation &r);
    void addSimulator(const Representation &r, bool use_behavior);

    void internalStep_internal();
    void stepAfterInput_internal();
    void processInput_internal();

    void debug_prolog(std::string what = "");
    void debug_epilog(std::string what = "");
    void debug_common();

    std::vector<const ISingle *> ifaces_;
    std::vector<const ISingle *> inIFaces_;
    std::vector<const ISingle *> outIFaces_;
    const ISingle *iClock_ = nullptr;
    const ISingle *iReset_ = nullptr;
    std::map<const Timing *, std::vector<std::unique_ptr<Simulator>>> sims_;
    std::string msgs_;
};

#endif // SIMTESTER_H
