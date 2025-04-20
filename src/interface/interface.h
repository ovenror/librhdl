#ifndef INTERFACE_H
#define INTERFACE_H

#include "interface/compositedirection.h"

#include "c_api/typedcobject.h"

#include "visitors/interfacevisitor.h"
#include "visitors/interfacecaster.h"
#include "cresult/compatibilityresult.h"

#include "construction/connectionpredicate.h"

#include "util/visitable.h"

#include <string>
#include <functional>
#include <queue>
#include <iostream>
#include <map>
#include <memory>

namespace rhdl {

class Entity;
class ISingle;
class IComposite;
class Interface;
class Predicate_2nd;

bool operator>=(const Interface &super, const Interface &sub);

class Interface :
		public TypedCObject<Interface, rhdl_iface_struct, false>,
		public AbstractVisitable<Interface, InterfaceVisitor>
{
public:
	using Direction = SingleDirection;
	using Predicate2 = ConnectionPredicate;
	using CResult = std::unique_ptr<CompatibilityResult>;

	enum class Type {SINGLE = RHDL_SINGLE, COMPOSITE = RHDL_COMPOSITE, PLACEHOLDER = RHDL_UNSPECIFIED};

	virtual ~Interface();

	Interface &cast() override {return *this;};

	virtual Interface *clone(const std::string &newName) const = 0;
	Interface *clone() const {
		return clone(name());
	}

	Type type() const;
	virtual CompositeDirection compositeDirection() const = 0;
	virtual SingleDirection preferredDirection() const = 0;

	template <class T>
	const T &cast();

	const Interface *operator[] (const std::string &iname) const;

	std::string qualifiedName(const Interface &top) const;
	std::vector<const ISingle *> flat() const;
	bool evalPredicate(Predicate_2nd predicate) const;
	const Interface& getCorrespondingSubInterface(
			const Interface &counterpart, const Interface &sub,
			const Predicate2 &pred) const;
	CResult compatTo(const Interface &other, const Predicate2 &predicate) const;
	bool compatibleTo(const Interface &, const Predicate2 &) const;

	template <class RESULT> RESULT checkCompatTo(
			const Interface &, const Predicate2 &) const;

	bool eq_name (const Interface &other) const;
	bool eq_names (const Interface &other) const;
	virtual bool eq_inner_names (const Interface &other) const = 0;
	virtual void add_components_to_queue(std::queue<const Interface *> &bfs_backlog) const;

	operator std::string() const;

protected:
	Interface(const std::string &name);

private:
	virtual const Interface *get(const std::string &name) const = 0;

public:
	static constexpr const char *anon_name = "<anonymous>";
};

std::ostream &operator<<(std::ostream &os, const Interface &i);

template<class T>
inline const T &Interface::cast()
{
	InterfaceCaster<T, true> caster;
	accept(caster);
	return caster();
}

}

#endif // INTERFACE_H
