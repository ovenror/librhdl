#include "setopenvisitor.h"
#include "../isingle.h"
#include "../icomposite.h"
#include "../iplaceholder.h"

namespace rhdl {

SetOpenVisitor::SetOpenVisitor()
{

}

void SetOpenVisitor::visit(const ISingle &i)
{
	i.setOpen();
}

void SetOpenVisitor::visit(const IComposite &i)
{
	for (const Interface *c : i) {
		c -> accept(*this);
	}
}

void SetOpenVisitor::visit(const IPlaceholder &i)
{
	i.realization() -> accept(*this);
}

}

