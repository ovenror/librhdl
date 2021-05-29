#ifndef INTERFACEDOUBLEVISITOR_H
#define INTERFACEDOUBLEVISITOR_H

#include "interfacevisitor.h"
#include "util/util.h"
#include "../iplaceholder.h"
#include <functional>
#include <cassert>

namespace rhdl {

template <bool CONST>
class InterfaceDoubleVisitor;

template <bool CONST, class INTERFACE>
class Collector : public InterfaceVisitor<CONST> {
public:
	using Super = InterfaceVisitor<CONST>;

	template <class T>
	using Visitee = typename Super::template Visitee<T>;

	Collector(Visitee<INTERFACE> &iface, InterfaceDoubleVisitor<CONST> &v) : iface_(iface), v_(v) {}

	void visit(Visitee<ISingle> &i) override {v_.visit(iface_, i);}
	void visit(Visitee<IComposite> &i) override {v_.visit(iface_, i);}
	void visit(Visitee<IPlaceholder> &i) override {v_.visit(iface_, i);}


private:
	Visitee<INTERFACE> &iface_;
	InterfaceDoubleVisitor<CONST> &v_;
};

class Interface;


template<bool CONST>
class InterfaceDoubleVisitor : public InterfaceVisitor<CONST>
{
public:
	using Super = InterfaceVisitor<CONST>;

	template <class T>
	using Visitee = typename Super::template Visitee<T>;

	InterfaceDoubleVisitor() : other_(nullptr) {}

	virtual void visit(Visitee<ISingle> &i1, Visitee<ISingle> &i2) = 0;

	virtual void visit(Visitee<IComposite> &i1, Visitee<IComposite> &i2) {visit_components(i1,i2);}

	virtual void visit(Visitee<IPlaceholder> &i1, Visitee<IPlaceholder> &i2) {visit(static_cast<Visitee<Interface> &>(i1), static_cast<Visitee<Interface> &>(i2));}
	virtual void visit(Visitee<Interface> &i1, Visitee<IPlaceholder> &i2) {visit(i1, static_cast<Visitee<Interface> &>(i2));}
	virtual void visit(Visitee<IPlaceholder> &i1, Visitee<Interface> &i2) {visit(static_cast<Visitee<Interface> &>(i1), i2);}
	virtual void visit(Visitee<Interface> &i1, Visitee<Interface> &i2) {std::ignore=i1;std::ignore=i2;assert(0);}

	void go_visit(Visitee<Interface> *i1, Visitee<Interface> *i2) {
		other_ = i2;
		i1 -> accept(*this);
	}

	void visit_components(Visitee<IComposite> &i1, Visitee<IComposite> &i2, std::function<bool()> done = [](){return false;})
	{
		auto ext_iter = i1.begin();
		auto int_iter = i2.begin();

		while (ext_iter != i1.end()) {
			assert (int_iter != i2.end());

			go_visit(*ext_iter, *int_iter);

			if (done())
				break;

			++ext_iter;
			++int_iter;
		}

		assert (int_iter == i2.end());
	}

	template <class INTERFACE>
	void collect(Visitee<INTERFACE> &i)
	{
		Collector<CONST, INTERFACE> collector(i, *this);
		other_ -> accept(collector);
	}

	void visit(Visitee<ISingle> &i) override {collect<ISingle>(i);}
	void visit(Visitee<IComposite> &i) override {collect<IComposite>(i);}
	void visit(Visitee<IPlaceholder> &i) override {collect<IPlaceholder>(i);}


private:
	Visitee<Interface> *other_;
};



}

#endif // INTERFACEDOUBLEVISITOR_H
