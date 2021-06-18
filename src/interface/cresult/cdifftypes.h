#ifndef CDIFFTYPES_H
#define CDIFFTYPES_H

#include "cbase.h"

namespace rhdl {

class Interface;

class CDiffTypesOps : public CSametypeOpsBase<Interface&, CDiffTypesOps>
{
	using Super::Super;
};

class CDiffTypes : public CBase<CDiffTypesOps>
{
public:
	using Super::Super;

	~CDiffTypes();

	void eval_int() const override;
	static bool compatible(const Interface &, const Interface &, const Predicate &)
		{return false;}
};

}


#endif // CDIFFTYPES_H
