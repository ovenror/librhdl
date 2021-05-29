#include <rhdl/construction/constructionexception.h>
#include "interface/icomposite.h"
#include "interface/predicate.h"
#include "interface/cresult/ccomposite.h"

#include <iostream>
#include <queue>

namespace rhdl {

IComposite::IComposite(const std::string &name)
	: VisitableBase (name)
{
	c_.content().type = RHDL_COMPOSITE;
	c_.content().composite.interfaces = c_strings_.data();
}

IComposite::IComposite(const IComposite &tmpl)
	: IComposite(tmpl.name_)
{
	for (const Interface *component : tmpl.components_)
	{
		add(component -> clone());
	}
}

IComposite::~IComposite()
{
	for (const Interface *component : components_)
	{
		delete component;
	}
}

IComposite *IComposite::clone() const
{
	return new IComposite (*this);
}

const Interface *IComposite::get(const std::string &name) const
{
	for (const Interface *interface : components_) {
		if (!name.compare (interface -> name())) {
			return interface;
		}
	}

	return nullptr;
}

const Interface *IComposite::find_connectible_components(const Interface *to, const Interface::Predicate2 &predicate) const
{
	const Interface *found = nullptr;
	//std::cerr << "   Composite: inspect own components" << std::endl;

	for (const Interface *interface : components_) {
		const Interface *current = interface -> find_connectible(to, predicate);

		if (!current)
			continue;

		if (found) {
			//std::cerr << "R(C~I): MULTIPLE" << std::endl;
			throw ConstructionException(Errorcode::E_FOUND_MULTIPLE_COMPATIBLE_INTERFACES);
			return nullptr;
		}

		found = current;
	}

	return found;
}

const Interface *IComposite::find_connectible(const Interface *to, const Predicate2 &predicate) const
{
	//std::cerr << name_ << " C~I: inspect self" << std::endl;

	const Interface *found = Super::find_connectible(to, predicate);

	if (found)
		return found;

	return find_connectible_components(to, predicate);
}

std::pair<const Interface *, const Interface *> IComposite::find_connectibles(const Interface *to, const Predicate2 &predicate) const
{
	//std::cerr << name_ << " C~I: super for each own component" << std::endl;

	std::queue <const Interface *> bfs_backlog;
	std::pair<const Interface *, const Interface *> found(0,0);

	bfs_backlog.push(this);

	while (!bfs_backlog.empty()) {
		const Interface *current = bfs_backlog.front();
		bfs_backlog.pop();
		auto current_result = current -> Super::find_connectibles(to, predicate);

		if (!current_result.first || !current_result.second) {
			current -> add_components_to_queue(bfs_backlog);
			continue;
		}

		if (found.first) {
			//std::cerr << "C~I: MULTIPLE" << std::endl;
			throw ConstructionException(Errorcode::E_FOUND_MULTIPLE_COMPATIBLE_INTERFACES);
			return {0,0};
		}

		found = current_result;
	}

	return found;
}

void IComposite::add_components_to_queue(std::queue<const Interface *> &bfs_backlog) const
{
	for (const Interface *component : components_) {
		bfs_backlog.push(component);
	}
}

Interface::CResult IComposite::eq_struct_int(const Interface &other, const Predicate2 &predicate) const
{
	//std::cerr << "generic struct C to I(" << typeid(other).name() << ") reverse comp struct" << std::endl;

	return other.eq_struct_int (*this, predicate.reversed());
}

Interface::CResult IComposite::eq_struct_int(const IComposite &other, const Predicate2 &predicate) const
{
	return CResult(new CComposite(*this, other, predicate));
}

void IComposite::add(const Interface *comp)
{
	components_.push_back(comp);
	c_strings_.back() = comp -> name().data();
	c_strings_.push_back(nullptr);
	c_.content().composite.interfaces = c_strings_.data();
}

bool IComposite::eq_inner_names(const Interface &other) const
{
	if (!eq_struct_int (other, Predicate2::ptp()) -> success())
	{
		return false;
	}

	/* Structural comparison beforehand guarantees type */
	IComposite *otta = (IComposite *) &other;

	for (size_t index = 0; index < components_.size (); ++index)
	{
		if (!components_ [index] -> eq_names (*otta -> components_ [index]))
		{
			return false;
		}
	}

	return true;
}

const char* const * rhdl::IComposite::ls() const {
	return c_strings_.data();
}

} // namespace rhdl

