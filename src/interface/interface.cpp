#include "interface/interface.h"
#include "interface/isingle.h"
#include "interface/icomposite.h"
#include "interface/predicate.h"
#include "interface/cresult/cdifftypes.h"

#include "visitors/correspondingsubinterfacefinder.h"
#include "visitors/flattener.h"
#include "visitors/predicateevaluator.h"
#include "visitors/qualifiednamemaker.h"
#include "visitors/compatchecker.h"

#include <iostream>
#include <cassert>

namespace rhdl {

Interface::Interface(
		const std::string &name) : TypedCObject(name)
{}

Interface::~Interface() {}

bool Interface::eq_name(const Interface &other) const
{
	return !(name().compare (other.name()));
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


template<class RESULT>
inline RESULT Interface::checkCompatTo(
		const Interface &i, const Predicate2 &p) const
{
	CompatChecker<RESULT> visitor(p);
	visitor.go_visit(this, &i);
	return visitor.result();
}

Interface::CResult Interface::compatTo(
	const Interface &i, const Predicate2 &p) const
{
	return checkCompatTo<CResult>(i, p);
}

bool Interface::compatibleTo(const Interface &i, const Predicate2 &p) const
{
	return checkCompatTo<bool>(i, p);
}

Interface::operator std::string() const
{
	return std::string("Interface '") + name() + "' (" + typeid(*this).name() + ")";
}


bool Interface::eq_names(const Interface &other) const
{
	return eq_name (other) && eq_inner_names (other);
}

bool Interface::evalPredicate(Predicate_2nd predicate) const {
	return PredicateEvaluator(std::move(predicate)).eval(*this);
}

bool operator>=(const Interface &super, const Interface &sub)
{
	const Interface *subptr = &sub;
	return super.evalPredicate(Exists(Predicate<Interface>([subptr](const Interface &i){return &i == subptr;})));
}

std::ostream &operator<<(std::ostream &os, const Interface &i)
{
	os << static_cast<const std::string &>(i);
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

Interface::Type Interface::type() const {
	return static_cast<Type>(c_.content_.type);
}

void Interface::add_components_to_queue(
		std::queue<const Interface*> &bfs_backlog) const
{
	std::ignore = bfs_backlog;
}

}
