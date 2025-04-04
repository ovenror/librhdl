#ifndef rhdl_ICOMPOSITE_H
#define rhdl_ICOMPOSITE_H


#include "interface.h"

#include "util/visitable.h"
#include "util/cached.h"

#include <vector>
#include <list>

namespace rhdl {

class IComposite : public Interface::VisitableChild<IComposite>
{
public:
	using Super = Interface;
	using InterfaceContainer =  std::vector<const Interface *>;
	using const_iterator = InterfaceContainer::const_iterator;

	IComposite(
			const std::string &name, std::vector<const Interface *> components);
	IComposite(const IComposite &tmpl);

	virtual ~IComposite();

	CompositeDirection compositeDirection() const override {return direction_;}
	SingleDirection preferredDirection() const override;
	IComposite *clone(const std::string &newName) const;

	bool eq_inner_names (const Interface &other) const;

	const_iterator begin() const {return components_.cbegin();}
	const_iterator end() const {return components_.cend();}
	size_t size() const {return components_.size();}
	void assert_not_empty() const {assert(!components_.empty());}

	static bool componentWise(
			const std::vector<const IComposite *> &,
			const std::function<bool(const std::vector<const Interface *> &)> &);

	bool componentWise(
			const IComposite &,
			const std::function<bool(const Interface &, const Interface &)> &f) const;

private:
	const Interface *get(const std::string &name) const override;

	InterfaceContainer components_;
	CompositeDirection direction_;
};

} // namespace rhdl

#endif // rhdl_ICOMPOSITE_H
