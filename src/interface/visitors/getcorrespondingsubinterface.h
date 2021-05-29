#ifndef GETCORRESPONDINGSUBINTERFACE_H
#define GETCORRESPONDINGSUBINTERFACE_H

#include "interfacedoublevisitor.h"
#include "../interface.h"
#include "../predicate.h"

namespace rhdl {

class Interface;

class GetCorrespondingSubinterface : public InterfaceDoubleVisitor<true>
{
public:
	GetCorrespondingSubinterface(
			//const Interface &target,
			//const Interface &reference,
			const Interface &sub, const Interface::Predicate2 &predicate);

	void visit(const ISingle &i1, const ISingle &i2)  override;
	void visit(const IComposite &i1, const IComposite &i2) override;
	void visit(const IPlaceholder &i1, const IPlaceholder &i2) override;
	void visit(const Interface &i1, const IPlaceholder &i2) override;
	void visit(const IPlaceholder &i1, const Interface &i2) override;

	bool check(const Interface &i1, const Interface &i2);
	const Interface *result() {return result_;}

private:
	//const Interface &target_;
	//const Interface &reference_;
	const Interface &sub_;
	const Interface::Predicate2 predicate_;
	const Interface *result_;
};
}


#endif // FINDCORRESPONDINGSUBINTERFACE_H
