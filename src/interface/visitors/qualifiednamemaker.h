#ifndef QNVISITOR_H
#define QNVISITOR_H

#include "interfacevisitor.h"

#include <string>

namespace rhdl {

class Interface;

class QualifiedNameMaker : public InterfaceVisitor<true>
{
public:
	QualifiedNameMaker(const Interface &target);

	//void visit(const Interface &i) override;
	void visit(const ISingle &i) override;
	void visit(const IComposite &i) override;

	void generic(const Interface &i);

	std::string result() {return result_;}

private:
	const Interface &target_;
	std::string result_;
};

}


#endif // QNVISITOR_H
