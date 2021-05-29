#include "simprinter.h"

#include "simulation/simulator.h"

#include "interface/isingle.h"

#include <cassert>
#include <iomanip>

namespace rhdl {

SimPrinter::SimPrinter(std::unique_ptr<rhdl::Simulator> &&sim, const std::vector<const ISingle *> &ifaces)
    : sim_(std::move(sim)), ifaces_(ifaces), historyIdx_(0)
{
}

SimPrinter::~SimPrinter()
{
    print();
}

void SimPrinter::processInput()
{
    sim_ -> processInput();
}

void SimPrinter::internalStep()
{
    sim_ -> internalStep();
    record();
}

void SimPrinter::record()
{
    assert (historyIdx_ <= length_);

    for (const ISingle *iface : ifaces_) {
        auto &entry = history_[iface][historyIdx_];

        if (iface -> direction() == Interface::Direction::IN)
            entry = sim_ -> getInput(iface) ? '1' : '0';
        else
            entry = sim_ -> get(iface) ? '1' : '0';
    }

    if (++historyIdx_ == length_) {
        print();
        historyIdx_ = 0;
    }

    assert (historyIdx_ < length_);
}

void SimPrinter::print()
{
    if (historyIdx_ == 0)
        return;

    assert (historyIdx_ <= length_);

    std::cout << typeid(*sim_).name() << std::endl;

    for (const ISingle *iface : ifaces_) {
        auto iname = iface -> name();

        //if (iface -> direction() == Interface::IN)
        //    iname += " (INP)";

        char *data =  history_.at(iface).data();
        data[historyIdx_] = 0;
        std::cout << std::setw(16) << iname << ": " << data << std::endl;
    }

    std::cout << std::endl;
}

void SimPrinter::toStream(std::ostream &os) const
{
    os << *sim_;
}

}
