#ifndef SIMFUZZER_H
#define SIMFUZZER_H

#include "simtester.h"
#include <random>

namespace rhdl {

class SimFuzzer
{
public:
    SimFuzzer(const rhdl::Entity &e);

    std::string getMsgs();

    bool run();
    void flip();

private:
    bool reset();
    bool functionalTest();
    bool stressTest();

    bool settle();
    void clkEdge();

    SimTester tester_;

    std::default_random_engine randgen_;
    std::uniform_int_distribution<int> uniform_;

    static constexpr unsigned int tSettle_ = 20;

    const Entity &entity_;
    bool clock_;
    bool clkState_;
};

}


#endif // SIMFUZZER_H
