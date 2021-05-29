#ifndef VISITABLE_H
#define VISITABLE_H

#include <utility>
#include <iostream>
#include "../util/util.h"

namespace rhdl {

template <class Derived, template <bool> class Visitor, class Base=Empty>
class Visitable : public Base
{
public:
	using VisitableBase = Visitable;
	using nonconst_Visitor = Visitor<false>;
	using const_Visitor = Visitor<true>;

	template<typename... Args>
	Visitable(Args&&... args) : Base(std::forward<Args>(args)...) {}

	Visitable() {}
	virtual ~Visitable() {}

	virtual void accept(Visitor<false> &v) override
	{
		//std::cerr << typeid(*this).name() << " accept()s " << typeid(v).name() << std::endl;
		//v.visit (*((Derived *) this));
		v.visit (*dynamic_cast<Derived *>(this));
	}

	virtual void accept(Visitor<true> &v) const override
	{
		//std::cerr << typeid(*this).name() << " accept()s " << typeid(v).name() << std::endl;
		//v.visit (*((const Derived *) this));
		v.visit (*dynamic_cast<const Derived *>(this));
	}
};

template <class AbstractVisitable, template <bool> class Visitor>
struct MakeDerivedVisitable {
template <class Derived, class Base=AbstractVisitable>
using type = Visitable<Derived, Visitor, Base>;
};

template <class Derived, template <bool> class Visitor /* could add base param if needed */ >
class AbstractVisitable
{
public:
	template <class NextDerived, class Base=Derived>
	using VisitableChild = typename MakeDerivedVisitable<Derived, Visitor>::template type<NextDerived, Base>;

	AbstractVisitable() {}
	virtual ~AbstractVisitable() {}

	virtual void accept(Visitor<false> &v) = 0;
	virtual void accept(Visitor<true> &v) const = 0;
};



}

#endif // VISITABLE_H
