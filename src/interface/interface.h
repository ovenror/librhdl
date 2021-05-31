#ifndef INTERFACE_H
#define INTERFACE_H

#include "interface/direction.h"

#include "visitors/interfacevisitor.h"
#include "cresult/compatibilityresult.h"

#include "construction/connectionpredicate.h"

#include "util/visitable.h"

#include "c_api/wrapper.h"

#include <string>
#include <functional>
#include <queue>
#include <iostream>
#include <map>
#include <memory>

namespace rhdl {

class ISingle;
class IComposite;
class Interface;

bool operator>=(const Interface &super, const Interface &sub);

class Interface : public AbstractVisitable<Interface, InterfaceVisitor>
{
public:
	using Direction = InterfaceDirection;
	using Predicate2 = ConnectionPredicate;

	using CResult = std::unique_ptr<CompatibilityResult>;

	Interface(const std::string &name = "");
	virtual ~Interface();

	virtual Interface *clone() const = 0;

	const std::string &name() const {return name_;}

	const Interface *operator[] (const std::string &iname) const;
	virtual const char *const *ls() const = 0;

	virtual const Interface *find_connectible(const Interface *to, const Predicate2 &predicate) const;
	virtual std::pair<const Interface *, const Interface *> find_connectibles(const Interface *to, const Predicate2 &predicate) const;
	virtual void add_components_to_queue(std::queue<const Interface *> &bfs_backlog) const;

	virtual CResult eq_struct(const Interface &other, const Predicate2 &predicate) const;
	virtual CResult eq_struct_int(const Interface &other, const Predicate2 &predicate) const = 0;
	virtual CResult eq_struct_int (const ISingle &other, const Predicate2 &predicate) const;
	virtual CResult eq_struct_int (const IComposite &other, const Predicate2 &predicate) const;

	bool is_partially_open() const;
	void setAllOpen() const;
	const Interface& getCorrespondingSubInterface(
			const Interface &counterpart, const Interface &sub,
			const Predicate2 &pred) const;

	bool eq_name (const Interface &other) const;
	bool eq_names (const Interface &other) const;
	virtual bool eq_inner_names (const Interface &other) const = 0;

	operator std::string() const;

private:
	virtual const Interface *get(const std::string &name) const = 0;

protected:
	std::string name_;


	friend class IPlaceholder;

public:
	using C_Struct = rhdl_iface_struct;
	static constexpr const char *anon_name = "<anonymous>";

protected:
	friend class Wrapper<Interface>;
	//friend class Wropper<rhdl_iface_struct>;
	static constexpr unsigned long C_ID = 0x197E5FACE;
	Wrapper<Interface> c_;
};

std::ostream &operator<<(std::ostream &os, const Interface &i);

}

#endif // INTERFACE_H
