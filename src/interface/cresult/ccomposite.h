#ifndef CCOMPOSITE_H
#define CCOMPOSITE_H

#include "cbase.h"

namespace rhdl {

class IComposite;

class CComposite : public CBase<IComposite>
{
public:
	using BASE = CBase<IComposite>;

	CComposite(const IComposite &from, const IComposite &to, const Predicate &predicate);
	~CComposite();

	void eval_int() const override;
};

}

#endif // CCOMPOSITE_H
