#include <interface/visitors/correspondingsubinterfacefinder.h>
#include "../isingle.h"
#include "../icomposite.h"

namespace rhdl {

CorrespondingSubInterfaceFinder::CorrespondingSubInterfaceFinder(
		const Interface &sub, const Interface::Predicate2 &predicate)
	:
		sub_(sub), predicate_(predicate), result_(nullptr)
{}

void CorrespondingSubInterfaceFinder::visit(const ISingle &i1, const ISingle &i2)
{
	check(i1, i2);
}

void CorrespondingSubInterfaceFinder::visit(const IComposite &i1, const IComposite &i2)
{
	if (check(i1, i2))
		return;

	visit_components(i1, i2, [this]() -> bool {return this->result_ != nullptr;});
}

bool CorrespondingSubInterfaceFinder::check(const Interface &i1, const Interface &i2)
{
	if (&i2 != &sub_)
		return false;

	if (!i1.compatTo(i2, predicate_))
		return false;

	result_ = &i1;
	return true;
}

}

