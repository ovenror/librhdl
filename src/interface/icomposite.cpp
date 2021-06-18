#include <rhdl/construction/constructionexception.h>
#include "interface/icomposite.h"
#include "interface/predicate.h"
#include "interface/cresult/ccomposite.h"

#include <iostream>
#include <queue>

namespace rhdl {

IComposite::IComposite(
		const std::string &name, std::vector<const Interface *> components)
	: VisitableBase (name), components_(components)
{
	c_.content().type = RHDL_COMPOSITE;

	for (auto *comp : components)
	{
		direction_ |= comp -> compositeDirection();
		c_strings_.push_back(comp -> name().data());
	}

	c_strings_.push_back(nullptr);
	c_.content().composite.interfaces = c_strings_.data();
}

static std::vector<const Interface *> cloneComponents(
		const std::vector<const Interface *> &ifaces)
{
	std::vector<const Interface *> result;

	for (auto *iface : ifaces) {
		result.push_back(iface -> clone());
	}

	return result;
}

IComposite::IComposite(const IComposite &tmpl)
	: IComposite(tmpl.name_, cloneComponents(tmpl.components_))
{}

IComposite::~IComposite()
{
	for (const Interface *component : components_)
	{
		delete component;
	}
}

IComposite *IComposite::clone(const std::string &newName) const
{
	return new IComposite (newName, cloneComponents(components_));
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

bool IComposite::eq_inner_names(const Interface &other) const
{
	if (!compatTo(other, Predicate2::ptp()) -> success())
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

SingleDirection IComposite::preferredDirection() const
{
	assert (!components_.empty());
	assert (!direction_.free());

	if (direction_.mixed())
		return components_.front() -> preferredDirection();

	return static_cast<SingleDirection>(direction_);
}

bool IComposite::componentWise(
		const IComposite &other,
		const std::function<bool(const Interface &, const Interface &)> &f) const
{
	assert (size() == other.size());

	auto c = begin();
	auto oc = other.begin();

	while (c != end()) {
		if (!f(**c++, **oc++))
			return false;
	}

	return true;
}

bool IComposite::componentWise(
		const std::vector<const IComposite *> &interfaces,
		const std::function<bool(const std::vector<const Interface *> &)> &f)
{
	if (interfaces.empty())
		return true;

	std::vector<const_iterator> iterators;
	std::vector<const Interface *> subs;

	const auto size = interfaces.size();

	subs.reserve(size);
	iterators.reserve(size);

	for (auto *iface : interfaces)
		iface -> assert_not_empty();

	auto iface = interfaces.begin();
	auto &first_iface = **iface;
	auto num_components = first_iface.size();

	for (; iface != interfaces.end(); ++iface)
	{
		const auto &riface = **iface;
		assert (riface.size() == num_components);

		auto iterator = riface.begin();
		subs.push_back(*iterator++);
		iterators.push_back(iterator);
	}

	const auto &first_iterator = iterators.front();

	while (true) {
		if (!f(subs))
			return false;

		if (first_iterator != first_iface.end())
			break;

		auto sub = subs.begin();
		auto iterator = iterators.begin();

		while (sub != subs.end())
			*sub++ = *(*iterator++)++;
	}

	return true;
}

} // namespace rhdl

