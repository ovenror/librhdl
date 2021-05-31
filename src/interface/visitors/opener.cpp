#include <interface/visitors/opener.h>
#include "../isingle.h"
#include "../icomposite.h"
#include "../iplaceholder.h"

namespace rhdl {

Opener::Opener()
{

}

void Opener::visit(const ISingle &i)
{
	i.setOpen();
}

void Opener::visit(const IComposite &i)
{
	for (const Interface *c : i) {
		c -> accept(*this);
	}
}

void Opener::visit(const IPlaceholder &i)
{
	i.realization() -> accept(*this);
}

}

