#include "qnvisitor.h"
#include "../isingle.h"
#include "../icomposite.h"
#include "../iplaceholder.h"

#include <iostream>

namespace rhdl {

QNVisitor::QNVisitor(const Interface &target) :
	target_(target)
{
}

bool QNVisitor::generic(const Interface &i)
{
	if (&i != &target_)
		return false;

	result_ += i.name();
	return true;
}

void QNVisitor::visit(const IComposite &i)
{
	//std::cerr << typeid(*this).name() << " visiting " << typeid(i).name() << std::endl;

	if (generic(i))
		return;

	for (const Interface* component : i)
	{
		component -> accept(*this);

		if (result_.empty())
			continue;

		result_ = i.name() + "." + result_;
		break;
	}
}

void QNVisitor::visit(const ISingle &i)
{
	//std::cerr << typeid(*this).name() << " visiting " << typeid(i).name() << std::endl;

	if (!generic(i))
		return;

	result_ += i.is_open()?"-":"|";
}

void QNVisitor::visit(const IPlaceholder &i)
{
	//std::cerr << typeid(*this).name() << " visiting " << typeid(i).name() << std::endl;
	if (generic(i)) {
		result_ = std::string("P(") + result_ +")";
		return;
	}

	i.realization() -> accept(*this);

	if (result_.empty())
		return;

	result_ = std::string("P>") + result_;
}


}

