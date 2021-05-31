#include <interface/visitors/correspondingsubinterfacefinder.h>
#include "../isingle.h"
#include "../icomposite.h"
#include "../iplaceholder.h"

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

void CorrespondingSubInterfaceFinder::visit(const IPlaceholder &i1, const IPlaceholder &i2)
{
	go_visit(i1.realization(), i2.realization());
}

void CorrespondingSubInterfaceFinder::visit(const Interface &i1, const IPlaceholder &i2)
{
	go_visit(&i1, i2.realization());
}

void CorrespondingSubInterfaceFinder::visit(const IPlaceholder &i1, const Interface &i2)
{
	go_visit(i1.realization(), &i2);
}

bool CorrespondingSubInterfaceFinder::check(const Interface &i1, const Interface &i2)
{
	if (&i2 != &sub_)
		return false;

	if (!i1.eq_struct(i2, predicate_))
		return false;

	result_ = &i1;
	return true;
}

}

