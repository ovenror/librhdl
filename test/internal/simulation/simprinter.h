#ifndef SIMHISTORY_H
#define SIMHISTORY_H

#include "simulation/simulator.h"

#include <map>
#include <memory>
#include <vector>
#include <cassert>
#include <iostream>

namespace rhdl {

class Simulator;

class SimPrinter : public Simulator
{
public:
    SimPrinter(std::unique_ptr<Simulator> &&sim, const std::vector<const ISingle *> &ifaces);
    ~SimPrinter();

    bool get(const ISingle *iface) const override {return sim_ -> get(iface);}
    void set(const ISingle *iface, bool value) override {sim_ -> set(iface, value);}
    void processInput() override;
    void internalStep() override;

protected:
    void toStream(std::ostream &os) const override;

private:
    constexpr static unsigned int length_ = 60;
    using History = std::map<const ISingle *, std::array<char, length_ + 1>>;

    void record();
    void print();

    std::unique_ptr<Simulator> sim_;
    std::vector<const ISingle *> ifaces_;
    History history_;
    unsigned int historyIdx_;
};

}

#endif // SIMHISTORY_H
