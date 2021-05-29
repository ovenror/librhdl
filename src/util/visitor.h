#ifndef VISITOR_H
#define VISITOR_H

#include <type_traits>
#include "util.h"

namespace rhdl {

#if 0
template <class BaseType, class... DerivedTypes>
class VisitorGenerator {

};

template <class... DerivedTypes>
class VisitorGenerator<None, DerivedTypes...> {

};



template <bool CONST, class T>
class AbstractVisitMethod1 {
	virtual void visit(typename cond_const<CONST, T>::type &param) = 0;
};

template <bool CONST, class T1, class T2>
class AbstractVisitMethod2 {
	virtual void visit(typename cond_const<CONST, T1>::type &param1, typename cond_const<CONST, T2>::type &param2) = 0;
};


template <class T>
class GetAbstractVisitMethod1
{
	using result = AbstractVisitMethod1<true, T>;
};



template <class... Types>
class MultiMixin : public Types... {};

template <class... Types>
class MultiMixin<LList<Types...> > : public Types... {};

template <class... Types>
using wat =
	MultiMixin<
		typename Flatten<
			typename Map<
				GetAbstractVisitMethod1,
				Types...
			>::result
		>::result
	>;
#endif


#if 0

template<class typelist, bool CONST=true>
class AbstractVisitMethods1;

template<bool CONST=true>
class AbstractVisitMethods1<None, CONST> {};

template<class Car, class Cdr, bool CONST>
class AbstractVisitMethods1<RList<Car, Cdr>, CONST> : public AbstractVisitMethods1<Cdr, CONST> {
	  virtual void visit(cond_const<Car>::type param) = 0;
};


template<class typelist1, class typelist2, bool CONST=true>
class AbstractVisitMethods2;

template<bool CONST>
class AbstractVisitMethods2<None, None, CONST> {};

template<class Car1, class Cdr1, class Car2, class Cdr2, bool CONST>
class AbstractVisitMethods2<RList<Car1, Cdr1>, RList<Car2, Cdr2>, CONST> : public AbstractVisitMethods2<RList<Car1, Cdr1>, Cdr2, CONST> {
	  virtual void visit(cond_const<Car1>::type p1, cond_const<Car2>::type p2) = 0;
};

#endif

}

#endif // VISITOR_H
