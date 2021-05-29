#include "evalpredicate.h"
#include "../icomposite.h"
#include "../iplaceholder.h"

namespace rhdl {

EvalPredicate::EvalPredicate(Predicate_2nd &&predicate) :
	predicate_(std::forward<Predicate_2nd>(predicate))
{

}

void EvalPredicate::visit(const ISingle &i)
{
	predicate_(i);
}

void EvalPredicate::visit(const IComposite &i)
{
	predicate_(i);

	for (const Interface *c : i) {
		if (predicate_.done())
			return;

		c -> accept(*this);
	}
}

void EvalPredicate::visit(const IPlaceholder &i)
{
	predicate_(i);
	i.realization() -> accept(*this);
}

bool EvalPredicate::eval(const Interface &i)
{
	i.accept(*this);
	return predicate_.result();
}




}

