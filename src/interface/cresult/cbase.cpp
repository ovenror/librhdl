#include "cbase.h"

namespace rhdl {

template<class IFACE>
CBase<IFACE>::CBase(const IFACE &from, const IFACE &to, const Predicate &predicate) :
	from_(from), to_(to), predicate_(predicate)
{
}

template CBase<Interface>::CBase(const Interface &, const Interface &, const Predicate &);
template CBase<ISingle>::CBase(const ISingle &, const ISingle &, const Predicate &);
template CBase<IComposite>::CBase(const IComposite &, const IComposite &, const Predicate &);

template<class IFACE>
CBase<IFACE>::~CBase() {};

template CBase<Interface>::~CBase();
template CBase<ISingle>::~CBase();
template CBase<IComposite>::~CBase();

}

