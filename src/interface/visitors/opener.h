#ifndef SETOPENVISITOR_H
#define SETOPENVISITOR_H

#include "interfacevisitor.h"

namespace rhdl {

class Opener : public InterfaceVisitor<true>
{
public:
	Opener();

	void visit(const ISingle &i) override;
	void visit(const IComposite &i) override;
	void visit(const IPlaceholder &i) override;
};

}


#endif // SETOPENVISITOR_H
