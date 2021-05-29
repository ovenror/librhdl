#ifndef BLOCKSIM_H
#define BLOCKSIM_H

#include "representation/blocks/blocks.h"

#include "redstone.h"

#include <vector>
#include <map>
#include <memory>
#include <queue>

namespace rhdl {



class BlockSim
{
public:
	using Vec = Blocks::Vec;
	using CVec = Blocks::CVec;
	using SimMap = std::map<Vec, std::unique_ptr<BlockSim> >;
	using Queue = std::queue<BlockSim *>;
	using index_t = Blocks::index_t;

	BlockSim(Queue &q);

	void init(const SimMap &smap, const Vec &pos, const Blocks& blocks);
	virtual void propagate() const;
	void update(char value);
	bool get() const {return value_;}
	virtual void set() {update(redstone::maxPower);}
	virtual void reset() {value_ = 0;}

	virtual Block type() const = 0;
	virtual bool update_internal(char value) = 0;

protected:
	virtual void init_internal(const SimMap &smap, const Vec &pos, const Blocks& blocks) = 0;
	void add(const SimMap &smap, const Vec &pos);
	void add(const SimMap &smap, const Vec &pos, Block btype);

	void addExisting(const SimMap &smap, const Vec &pos);

	char value_;
	std::vector<BlockSim *> next_;
	Queue &queue_;

private:
	friend std::ostream &operator<<(std::ostream &os, const BlockSim &bs);

	Vec debug_pos_;
};

std::ostream &operator<<(std::ostream &os, const BlockSim &bs);

}

#endif // BLOCKSIM_H
