#ifndef INTERFACEVISITOR_H
#define INTERFACEVISITOR_H

#include <type_traits>
#include "util/util.h"

namespace rhdl {

//class Interface;
class ISingle;
class IComposite;
class IPlaceholder;

template <bool CONST=true>
class InterfaceVisitor
{
public:
	InterfaceVisitor() {};
	virtual ~InterfaceVisitor() {};

	template <class T>
	using Visitee = typename cond_const<CONST, T>::type;

	virtual void visit(Visitee<ISingle> &) = 0;
	virtual void visit(Visitee<IComposite> &) = 0;
	virtual void visit(Visitee<IPlaceholder> &) = 0;
};

}


#endif // INTERFACEVISITOR_H
