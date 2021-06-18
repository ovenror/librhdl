#ifndef BLOCKSSIM_H
#define BLOCKSSIM_H

#include "representation/blocks/blocks.h"
#include "exposableinternalstatesim.h"
#include "simulation/blocks/blocksim.h"

#include <queue>
#include <vector>
#include <array>
#include <iostream>

namespace rhdl::blocks {

class BSRedstone;
class BSDelay;

class BlocksSim : public ExposableInternalStateSim
{
public:
	using Vec = Blocks::Vec;

	BlocksSim(const Blocks &blocks);

	bool get(const ISingle *iface) const override;

	void processInput() override ;
	void internalStep() override;

	void test(const std::vector<Vec> &onPositions);
	bool extendTest();

	bool torchWasReached(const Vec &torchPos);
	bool blockWasReached(const Vec &pos);

protected:
	void setInternal(const ISingle *iface) override;

private:
	using Queue = BlockSim::Queue;
	using SimMap = BlockSim::SimMap;

	void reset();
	void propagate();
	void swtch();   

	template <class Sim> std::unique_ptr<BlockSim> makeDelaySim();
	std::vector<const ISingle *> getIfaces(const Blocks &blocks) const;

	BlockSim &getSim(const Vec &pos);

	SimMap allSims_;
	std::vector <BSDelay *> delaySims_;
	std::map<const ISingle *, BlockSim *> isims_;
	std::map<const ISingle *, bool> istate_;
	Queue queue_;

	Vec debug_dims_;

	friend std::ostream &operator<<(std::ostream &os, const BlocksSim &bs);
};

std::ostream &operator<<(std::ostream &os, const BlocksSim &bs);

}

#endif // BLOCKSSIM_H
