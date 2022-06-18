#include "blockssim.h"
#include "interface/isingle.h"
#include "util/marray_slicing_3D.h"
#include "blocks/bsredstone.h"
#include "blocks/bstorch.h"
#include "blocks/bsrepeater.h"
#include "blocks/bsopaque.h"

#include <set>

namespace rhdl::blocks {

BlocksSim::BlocksSim(const Blocks &blocks)
{
	init(getIfaces(blocks));

	auto dims = blocks.dimensions();
	debug_dims_ = dims;

	Vec pos = {-1,0,0};

	while (true) {
		unsigned int idx = 0;
		for (; idx <=2; ++idx) {
			if (++pos[idx] < dims[idx])
				break;

			pos[idx] = 0;
		}
		if (idx > 2)
			break;

		Block b = blocks[pos];
		std::unique_ptr<BlockSim> sim;

		switch (b) {
		case Block::REDSTONE:
			sim = std::make_unique<BSRedstone>(queue_);
			break;
		case Block::TORCH:
			//std::cerr << "torchsim at " << pos << std::endl;
			sim = std::move(makeDelaySim<BSTorch>());
			break;
		case Block::REPEATER:
			sim = std::move(makeDelaySim<BSRepeater>());
			break;
		case Block::OPAQUE:
			sim = std::make_unique<BSOpaque>(queue_);
			break;
		default:
			continue;
		}

		allSims_[pos] = std::move(sim);
	}

	for (auto &kv : allSims_) {
		kv.second -> init(allSims_, kv.first, blocks);
	}

	for (const ISingle *iface : getIfaces(blocks)) {
		Vec pos = blocks.ifaceMap().at(iface);
		isims_[iface] = allSims_.at(pos).get();
	}
}

bool BlocksSim::get(const ISingle *iface) const
{
	return isims_.at(iface) -> get();
}

void BlocksSim::setInternal(const ISingle *iface)
{
	isims_.at(iface) -> set();
}

void BlocksSim::reset()
{
	for (auto &kv : allSims_) {
		kv.second -> reset();
	}
}

void BlocksSim::propagate()
{
	while (!queue_.empty()) {
		BlockSim *sim = queue_.front();
		queue_.pop();
		sim -> propagate();
	}
}

void BlocksSim::swtch()
{
	for (BSDelay *delay: delaySims_)
		delay -> swtch();
}

void BlocksSim::internalStep()
{
	//std::cerr << *this;

	swtch();
	reset();
	propagate();
}

BlockSim &BlocksSim::getSim(const BlocksSim::Vec &pos)
{
	auto isim = allSims_.find(pos);
	assert (isim != allSims_.end());
	return *(isim -> second);
}

void BlocksSim::test(const std::vector<BlocksSim::Vec> &onPositions)
{
	std::set<BlockSim *> torchSimSet;

	for (const Vec &v : onPositions) {
		BlockSim &sim = getSim(v);

		if (sim.type() != Block::TORCH)
			continue;

		assert(torchSimSet.insert(&sim).second);
	}

	reset();

	unsigned int foundTorches = 0;
	for (BSDelay *delaySim : delaySims_) {
		auto iSim = torchSimSet.find(delaySim);

		if (delaySim -> type() != Block::TORCH)
			continue;

		if (iSim == torchSimSet.end()) {
			delaySim -> update(1);
			continue;
		}

		++foundTorches;
		delaySim -> update(0); //not necessary?
	}
	assert (foundTorches == torchSimSet.size());

	swtch();
	reset();

	for (const Vec &v : onPositions) {
		BlockSim &sim = getSim(v);

		switch (sim.type()) {
		case Block::REDSTONE:
			sim.set();
			break;
		case Block::TORCH:
			break;
		default: assert(0);
		}
	}

	propagate();
	//std::cerr << *this;
}

bool BlocksSim::extendTest()
{
	for (BSDelay *delaySim : delaySims_)
		if (delaySim -> type() == Block::TORCH)
			delaySim -> update(1);

	swtch();

	bool still_active = false;

	for (BSDelay *delaySim : delaySims_) {
		if (delaySim -> type() != Block::REPEATER)
			continue;

		if (!delaySim -> get())
			continue;

		still_active = true;
		break;
	}

	if (!still_active)
		return false;

	reset();
	propagate();

	//std::cerr << *this;

	return true;
}

bool BlocksSim::torchWasReached(const BlocksSim::Vec &torchPos)
{
	BlockSim &sim = getSim(torchPos);
	assert (sim.type() == Block::TORCH);
	auto iTorchSim = std::find(delaySims_.begin(), delaySims_.end(), &sim);
	assert (iTorchSim != delaySims_.end());

	return !(*iTorchSim) -> onAfterSwitch();
}

bool BlocksSim::blockWasReached(const BlocksSim::Vec &pos)
{
	BlockSim &sim = getSim(pos);
	assert (sim.type() == Block::OPAQUE || sim.type() == Block::REDSTONE);
	return sim.get();
}

void BlocksSim::processInput()
{
	reset(); //TODO: necessary?
	ExposableInternalStateSim::processInput();
	propagate();
}

template <class Sim>
std::unique_ptr<BlockSim> BlocksSim::makeDelaySim()
{
	auto delaySim = std::make_unique<Sim>(queue_);
	delaySims_.push_back(delaySim.get());
	return delaySim;
}

std::vector<const ISingle *> BlocksSim::getIfaces(const Blocks &blocks) const
{
	std::vector<const ISingle *> result;

	for (auto &kv : blocks.ifaceMap()) {
		result.push_back(kv.first);
	}

	return result;
}

std::ostream &operator<<(std::ostream &os, const BlocksSim &bs)
{
	BlockSim::Vec dims = bs.debug_dims_;
	boost::multi_array<char, 2> img(boost::extents[dims[1]][dims[2]]);

	for (BlockSim::index_t x = 0; x < dims[1]; ++x)
		for (BlockSim::index_t y = 0; y < dims[2]; ++y)
			img[x][y] = ' ';

	for (auto &kv : bs.allSims_) {
		BlockSim::Vec pos = kv.first;
		auto &sim = kv.second;

		Block type = sim -> type();

		switch (type) {
		case Block::REDSTONE:
		case Block::REPEATER:
		case Block::TORCH:
			break;
		default:
			continue;
		}

		bool on = sim -> get();
		char &pixel = img[pos[1]][pos[2]];

		switch (type) {
		case Block::REDSTONE:
			pixel = on ? '*' : '.';
			break;
		case Block::TORCH:
			pixel = on ? '0' : 'o';
			break;
		case Block::REPEATER:
			pixel = on ? 'R' : 'r';
			break;
		default:
			assert (0);
		}
	}

	for (BlockSim::index_t x = 0; x < dims[1]; ++x) {
		for (BlockSim::index_t y = 0; y < dims[2]; ++y) {
			os << img[x][y];
		}
		os << std::endl;
	}

	return os;
}

}
