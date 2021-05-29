#include "flattenvisitor.h"
#include "interface/icomposite.h"

namespace rhdl {

FlattenVisitor::FlattenVisitor(std::vector<const ISingle *> &flat_interface) :
	flat_interface_(flat_interface)
{
}

void FlattenVisitor::visit(const ISingle &i)
{
	flat_interface_.push_back(&i);
}

void FlattenVisitor::visit(const IComposite &i)
{
	 for (const Interface *c : i) {
		c -> accept(*this);
	}
}

}
