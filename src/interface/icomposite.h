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

	IComposite(const std::string &name = "");
	IComposite(const IComposite &tmpl);

	~IComposite();

	IComposite *clone() const;

	const char *const *ls() const override;

	const Interface *find_connectible(const Interface *to, const Predicate2 &predicate) const override;
	std::pair<const Interface *, const Interface *> find_connectibles(const Interface *to, const Predicate2 &predicate) const override;
	void add_components_to_queue(std::queue<const Interface *> &bfs_backlog) const override;

	void add (const Interface *comp);

	bool eq_inner_names (const Interface &other) const;

	InterfaceContainer::const_iterator begin() const {return components_.cbegin();}
	InterfaceContainer::const_iterator end() const {return components_.cend();}

protected:
	const Interface *find_connectible_components(const Interface *to, const Interface::Predicate2 &predicate) const;

	virtual CResult eq_struct_int (const Interface &other, const Predicate2 &predicate) const;
	virtual CResult eq_struct_int (const IComposite &other, const Predicate2 &predicate) const;

private:
	const Interface *get(const std::string &name) const override;

	friend class IPlaceholder;
	friend class CComposite;
	using CStrings = std::vector<const char *>;

	InterfaceContainer components_;
	CStrings c_strings_ = {nullptr};
};

} // namespace rhdl

#endif // rhdl_ICOMPOSITE_H
