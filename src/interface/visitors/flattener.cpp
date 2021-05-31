#include <interface/visitors/flattener.h>
#include "interface/icomposite.h"

namespace rhdl {

Flattener::Flattener(std::vector<const ISingle *> &flat_interface) :
	flat_interface_(flat_interface)
{
}

void Flattener::visit(const ISingle &i)
{
	flat_interface_.push_back(&i);
}

void Flattener::visit(const IComposite &i)
{
	 for (const Interface *c : i) {
		c -> accept(*this);
	}
}

}
