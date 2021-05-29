#ifndef QNVISITOR_H
#define QNVISITOR_H

#include "interfacevisitor.h"
#include <string>

namespace rhdl {

class Interface;

class QNVisitor : public InterfaceVisitor<true>
{
public:
	QNVisitor(const Interface &target);

	//void visit(const Interface &i) override;
	void visit(const ISingle &i) override;
	void visit(const IComposite &i) override;
	void visit(const IPlaceholder &i) override;

	bool generic(const Interface &i);

	std::string result() {return result_;}

private:
	const Interface &target_;
	std::string result_;
};

}


#endif // QNVISITOR_H
