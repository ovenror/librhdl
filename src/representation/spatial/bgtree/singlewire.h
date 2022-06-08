#ifndef SINGLEWIRE_H
#define SINGLEWIRE_H

#include "wire.h"
#include "crosser.h"

namespace rhdl {
namespace TM {

class CollectedWire;

class SingleWire : public Wire, public Crosser
{
public:
	SingleWire(Container &owner, bool vertical, const std::shared_ptr<Connection> &connection, bool anchor = false);
	SingleWire(Container &owner, bool vertical, bool anchor = false);
	SingleWire(std::unique_ptr<CollectedWire> &&collected);

	Wires *collection() const override {return nullptr;}
	bool vertical() const override {return vertical_;}

	bool isConnectedTo(const Wire &w) const override {return Wire::isConnected(w);}
	const Wire *getWireConnectedTo(const Wire &w) const override;
	Wire *getWireConnectedTo(const Wire &w) override;

	Crosser &asCrosser() override {return *this;}
	const Crosser &asCrosser() const override {return *this;}

	const std::set<Crosser *> &getCrossers() const override {return Crosser::getCrossers();}

	void tryInsertCrosser(Wire &crosser) override;
	void replaceCrosser(Wire &newCrosser) override;
	bool mayBeCrossed(const Wire &wire, blocks::Blocks::index_t at) const override;

	const std::vector<const Wire *> debug_getWires() const override;
};


}
}

#endif // SINGLEWIRE_H
