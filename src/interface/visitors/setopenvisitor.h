#ifndef SETOPENVISITOR_H
#define SETOPENVISITOR_H

#include "interfacevisitor.h"

namespace rhdl {

class SetOpenVisitor : public InterfaceVisitor<true>
{
public:
	SetOpenVisitor();

	void visit(const ISingle &i) override;
	void visit(const IComposite &i) override;
	void visit(const IPlaceholder &i) override;
};

}


#endif // SETOPENVISITOR_H
