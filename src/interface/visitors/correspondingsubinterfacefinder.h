#ifndef GETCORRESPONDINGSUBINTERFACE_H
#define GETCORRESPONDINGSUBINTERFACE_H

#include "interfacedoublevisitor.h"
#include "../interface.h"
#include "../predicate.h"

namespace rhdl {

class Interface;

class CorrespondingSubInterfaceFinder : public InterfaceDoubleVisitor<true>
{
public:
	CorrespondingSubInterfaceFinder(
			const Interface &sub, const Interface::Predicate2 &predicate);

	void visit(const ISingle &i1, const ISingle &i2)  override;
	void visit(const IComposite &i1, const IComposite &i2) override;

	bool check(const Interface &i1, const Interface &i2);
	const Interface *result() {return result_;}

private:
	const Interface &sub_;
	const Interface::Predicate2 predicate_;
	const Interface *result_;
};
}


#endif // FINDCORRESPONDINGSUBINTERFACE_H
