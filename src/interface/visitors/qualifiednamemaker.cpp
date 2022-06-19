#include <interface/visitors/qualifiednamemaker.h>
#include "../isingle.h"
#include "../icomposite.h"

#include <iostream>

namespace rhdl {

QualifiedNameMaker::QualifiedNameMaker(const Interface &target) :
	target_(target)
{
}

void QualifiedNameMaker::generic(const Interface &i)
{
	assert (result_.empty());
	result_ = i.name();
}

void QualifiedNameMaker::visit(const IComposite &i)
{
	if (&i == &target_) {
		generic(i);
		return;
	}

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
	generic(i);
}

}

