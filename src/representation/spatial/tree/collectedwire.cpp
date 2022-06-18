#include "../tree/collectedwire.h"

#include "../tree/wires.h"

namespace rhdl {
namespace TM {

CollectedWire::CollectedWire(Container &owner, const std::shared_ptr<Connection> &connection, bool anchor, Wires &collection, unsigned int index) :
	CollectedWire(owner, anchor, collection, index)
{
	connect(connection);
}

CollectedWire::CollectedWire(Container &owner, bool anchor, Wires &collection, unsigned int index) :
	Wire(owner, anchor, index), collection_(collection)
{
	relativePosition_ = index * 2;
}

bool CollectedWire::vertical() const
{
	return collection_.vertical_;
}

Crosser &CollectedWire::asCrosser()
{
	return collection_;
}

const Crosser &CollectedWire::asCrosser() const
{
	return collection_;
}

const std::set<Crosser *> &CollectedWire::getCrossers() const
{
	return collection_.getCrossers();
}

void CollectedWire::dropHorizontalCollected()
{
	if (vertical())
		return;

	//std::cerr << "a.drop " << *this << std::endl;

	if (anchor_) {
		relativePosition_ = 0;
		return;
	}

	blocks::Blocks::index_t pos = 0;

	for (; pos < relativePosition_; pos += 2) {
		auto iter = collection_.begin();

		for (;iter != collection_.end(); ++iter) {
			if ((*iter) -> anchor_)
				continue;

			//if (iter -> get() == this)
			//    break;

			if (collection_.dropCollision(*this, pos, **iter))
				break;
		}

		//if (iter -> get() == this)
		if (iter == collection_.end())
			break;
	}

	relativePosition_ = pos;
	//std::cerr << "p.drop " << *this << std::endl;
}


}
}
