#ifndef INTERFACECASTER_H
#define INTERFACECASTER_H

#include "interfacevisitor.h"
#include "interface/interface.h"

#include <cassert>

namespace rhdl {

class Interface;

template<bool CONST>
class InterfaceCasterBase : public InterfaceVisitor<CONST>
{
public:
	InterfaceCasterBase();

	template <class T>
	using Visitee = typename InterfaceVisitor<CONST>::template Visitee<T>;

	void visit(Visitee<ISingle> &i) override {assert(0);}
	void visit(Visitee<IComposite> &i) override {assert(0);}
};

template<class T, bool CONST=true>
class InterfaceCaster : public InterfaceCasterBase<CONST>
{
public:
	InterfaceCaster() : result_(nullptr) {}

	using CastResult = typename InterfaceCasterBase<CONST>::template Visitee<T>;
	using Castee = typename InterfaceCasterBase<CONST>::template Visitee<Interface>;

	CastResult * operator()(Castee *i)
	{
		i -> accept(*this);
		return result_;
	}

	void visit(CastResult &i) override
	{
		result_ = &i;
	}

private:
	CastResult *result_;
};

}

#endif // INTERFACECASTER_H
