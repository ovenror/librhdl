#include "interface/interface.h"
#include "interface/isingle.h"
#include "interface/icomposite.h"
#include "interface/predicate.h"
#include "interface/cresult/cdifftypes.h"

#include "visitors/correspondingsubinterfacefinder.h"
#include "visitors/flattener.h"
#include "visitors/opener.h"
#include "visitors/predicateevaluator.h"
#include "visitors/qualifiednamemaker.h"

#include <iostream>
#include <cassert>

namespace rhdl {

Interface::Interface(const std::string &name)
	: name_ (name), c_(*this)
{}

Interface::~Interface()
{

}

const Interface *Interface::find_connectible(const Interface *to, const Predicate2 &predicate) const
{
	//std::cerr << "generic find connectible " << typeid(*this).name() << " to " << typeid(*to).name() << " comp struct" << std::endl;

	if (eq_struct(*to, predicate) -> success())
		return this;

	return nullptr;
}

std::pair<const Interface *, const Interface *> Interface::find_connectibles(const Interface *to, const Predicate2 &predicate) const
{
	//std::cerr << "I~I: reverse & reduce" << std::endl;

	const Interface *found = to -> find_connectible(this, predicate.reversed());

	if (!found)
		return {0,0};

	return {this, found};
}

void Interface::add_components_to_queue(std::queue<const Interface *> &bfs_backlog) const
{
	std::ignore = bfs_backlog;
}

bool Interface::eq_name(const Interface &other) const
{
	return !(name_.compare (other.name_));
}

Interface::CResult Interface::eq_struct(const Interface &other, const Interface::Predicate2 &predicate) const
{
	CResult result(eq_struct_int(other, predicate));

	//std::cerr << "Structural comparison of " << (std::string) *this << "@" << this << " and " << (std::string) other << " @" << &other << std::endl;
	//std::cerr << result -> error_msg();

	return result;
}

Interface::CResult Interface::eq_struct_int(const ISingle &other, const Predicate2 &predicate) const
{
	return CResult(new CDiffTypes(*this, other, predicate));
}

Interface::CResult Interface::eq_struct_int(const IComposite &other, const Predicate2 &predicate) const
{
	return CResult(new CDiffTypes(*this, other, predicate));
}

void Interface::setAllOpen() const {
	accept(Opener());
}

std::vector<const ISingle*> Interface::flat() const {
	std::vector<const ISingle *> result;
	accept(Flattener(result));
	return result;
}

std::string Interface::qualifiedName(const Interface &top) const {
	QualifiedNameMaker qnmaker(*this);
	top.accept(qnmaker);
	return qnmaker.result();
}

Interface::operator std::string() const
{
	return std::string("Interface '") + name_ + "' (" + typeid(*this).name() + ")";
}


bool Interface::eq_names(const Interface &other) const
{
	return eq_name (other) && eq_inner_names (other);
}

bool Interface::evalPredicate(Predicate_2nd predicate) const {
	return PredicateEvaluator(std::move(predicate)).eval(*this);
}

bool Interface::is_partially_open() const
{
	return evalPredicate(Exists(Predicate<ISingle>([](const ISingle &i){return i.is_open();})));
}

bool operator>=(const Interface &super, const Interface &sub)
{
	const Interface *subptr = &sub;
	return super.evalPredicate(Exists(Predicate<Interface>([subptr](const Interface &i){return &i == subptr;})));
}

std::ostream &operator<<(std::ostream &os, const Interface &i)
{
	os << (std::string) i;
	return os;
}

const Interface* Interface::operator [](const std::string& iname) const {
	if (iname == anon_name)
		throw ConstructionException(Errorcode::E_CANNOT_GET_ANONYMOUS_INTERFACE);

	return get(iname);
}

const Interface &Interface::getCorrespondingSubInterface
	(const Interface& counterpart, const Interface& sub, const Predicate2 &pred) const
{
	CorrespondingSubInterfaceFinder finder(sub, pred);
	finder.go_visit(this, &counterpart);

	const Interface *result = finder.result();
	assert(result);
	return *result;
}

}
