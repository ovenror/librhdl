#include <interface/visitors/predicateevaluator.h>
#include "../icomposite.h"

namespace rhdl {

PredicateEvaluator::PredicateEvaluator(Predicate_2nd &&predicate) :
	predicate_(std::forward<Predicate_2nd>(predicate))
{

}

void PredicateEvaluator::visit(const ISingle &i)
{
	predicate_(i);
}

void PredicateEvaluator::visit(const IComposite &i)
{
	predicate_(i);

	for (const Interface *c : i) {
		if (predicate_.done())
			return;

		c -> accept(*this);
	}
}

bool PredicateEvaluator::eval(const Interface &i)
{
	i.accept(*this);
	return predicate_.result();
}




}

