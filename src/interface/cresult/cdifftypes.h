#ifndef CDIFFTYPES_H
#define CDIFFTYPES_H

#include "cbase.h"

namespace rhdl {

class Interface;

class CDiffTypes : public CBase<Interface>
{
public:
	using BASE = CBase<Interface>;

	CDiffTypes(const Interface &from, const Interface &to, const Predicate &predicate);
	~CDiffTypes();

	//bool success() const override {return false;}
	void eval_int() const override;
};

}


#endif // CDIFFTYPES_H
