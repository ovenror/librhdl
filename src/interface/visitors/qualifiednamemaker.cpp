#include <interface/visitors/qualifiednamemaker.h>
#include "../isingle.h"
#include "../icomposite.h"
#include "../iplaceholder.h"

#include <iostream>

namespace rhdl {

QualifiedNameMaker::QualifiedNameMaker(const Interface &target) :
	target_(target)
{
}

bool QualifiedNameMaker::generic(const Interface &i)
{
	if (&i != &target_)
		return false;

	result_ += i.name();
	return true;
}

void QualifiedNameMaker::visit(const IComposite &i)
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

void QualifiedNameMaker::visit(const ISingle &i)
{
	//std::cerr << typeid(*this).name() << " visiting " << typeid(i).name() << std::endl;

	if (!generic(i))
		return;

	result_ += i.is_open()?"-":"|";
}

void QualifiedNameMaker::visit(const IPlaceholder &i)
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

