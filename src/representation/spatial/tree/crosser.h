#ifndef CROSSER_H
#define CROSSER_H

#include "representation/blocks/blocks.h"
#include <set>

namespace rhdl {
namespace spatial {

class Wire;
class CollectedWire;
class Container;

class Crosser
{
public:
	Crosser(bool vertical);
	Crosser(CollectedWire &&dying);

	virtual bool isConnectedTo(const Wire &w) const = 0;
	virtual const Wire *getWireConnectedTo(const Wire &w) const = 0;
	virtual Wire *getWireConnectedTo(const Wire &w) = 0;

	void addCrosser(Crosser &crosser);
	bool isCrosser(const Crosser &crosser) const;

	virtual void tryInsertCrosser(Wire &crosser) = 0;
	virtual void replaceCrosser(Wire &newCrosser) = 0;
	virtual bool mayBeCrossed(const Wire &wire, blocks::Blocks::index_t at) const = 0;

	const std::set<Crosser *> &getCrossers() const {return crossers_;}

	virtual const std::vector<const Wire *> debug_getWires() const = 0;

	const bool vertical_;

protected:
	void addCrosser_oneway(Crosser &crosser);

private:
	bool isCrosser_internal(const Crosser &crosser) const;

	std::set<Crosser *> crossers_;
};

}
}

#endif // CROSSABLE_H
