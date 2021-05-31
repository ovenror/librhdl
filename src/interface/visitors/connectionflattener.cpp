#include <interface/visitors/connectionflattener.h>

namespace rhdl {

ConnectionFlattener::ConnectionFlattener(
		std::vector<Structural::FlatConnection> &flat_connections,
		PartIdx from, PartIdx to)
:
	flat_connections_(flat_connections), from_(from), to_(to)
{}

void ConnectionFlattener::visit(const ISingle &i1, const ISingle &i2)
{
	flat_connections_.push_back({{from_, &i1}, {to_, &i2}});
}

}
