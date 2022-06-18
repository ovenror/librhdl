#include "../tree/singlewire.h"

#include "../tree/collectedwire.h"
#include "../tree/wires.h"

namespace rhdl {
namespace TM {

using blocks::Blocks;

SingleWire::SingleWire(Container &owner, bool vertical, const std::shared_ptr<Connection> &connection, bool anchor) :
	SingleWire(owner, vertical, anchor)
{
	connect(connection);
}

SingleWire::SingleWire(Container &owner, bool vertical, bool anchor) :
	Wire(owner, anchor), Crosser(vertical)
{
}

SingleWire::SingleWire(std::unique_ptr<CollectedWire> &&collected) :
	Wire(std::move(*collected), collected -> collection() -> owner_), Crosser(std::move(*collected))
{
	if (!hasPosition())
		return;

	assert (0);
}

const Wire *SingleWire::getWireConnectedTo(const Wire &w) const
{
	return isConnected(w) ? this : nullptr;
}

Wire *SingleWire::getWireConnectedTo(const Wire &w)
{
	return isConnected(w) ? this : nullptr;
}

void SingleWire::tryInsertCrosser(Wire &crosser)
{
	tryInsertCrosser_internal(crosser);
}

void SingleWire::replaceCrosser(Wire &newCrosser)
{
	replaceCrosser_internal(newCrosser);
}

bool SingleWire::mayBeCrossed(const Wire &wire, Blocks::index_t at) const
{
	return mayBeCrossed_internal(wire, at);
}

const std::vector<const Wire *> SingleWire::debug_getWires() const
{
	return std::vector<const Wire *>{this};
}

}
}
