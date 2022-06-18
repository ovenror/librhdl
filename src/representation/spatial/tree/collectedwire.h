#ifndef COLLECTEDWIRE_H
#define COLLECTEDWIRE_H

#include "../tree/wire.h"

namespace rhdl {
namespace TM {

class CollectedWire : public Wire
{
public:
	CollectedWire(Container &owner,
				  const std::shared_ptr<Connection> &connection, bool anchor,
				  Wires &collection, unsigned int index);

	CollectedWire(Container &owner, bool anchor, Wires &collection,
				  unsigned int index);

	Wires *collection() const override {return &collection_;}
	const Wires &theCollection() const {return collection_;}
	bool vertical() const override;

	Crosser &asCrosser() override;
	const Crosser &asCrosser() const override;

	const std::set<Crosser *> &getCrossers() const override;

	void dropHorizontalCollected() override;

	bool operator<(const CollectedWire &other) const;

protected:
	Wires &collection_;
};

}
}

#endif // COLLECTEDWIRE_H
