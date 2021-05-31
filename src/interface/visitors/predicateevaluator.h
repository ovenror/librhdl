#ifndef EVALPREDICATE_H
#define EVALPREDICATE_H

#include "interfacevisitor.h"
#include "../predicate.h"

namespace rhdl {

class PredicateEvaluator : public InterfaceVisitor<true>
{
public:
	PredicateEvaluator(Predicate_2nd &&predicate);

	void visit(const ISingle &i) override;
	void visit(const IComposite &i) override;
	void visit(const IPlaceholder &i) override;

	bool eval(const Interface &i);

private:
	Predicate_2nd predicate_;
};


}


#endif // EVALPREDICATE_H
