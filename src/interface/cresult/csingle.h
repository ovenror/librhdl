#ifndef CSINGLE_H
#define CSINGLE_H

#include "cbase.h"
#include "interface/interface.h"
#include <string>

namespace rhdl {

class ISingle;

class CSingle : public CBase<ISingle>
{
public:
	using BASE = CBase<ISingle>;
	using Predicate = Interface::Predicate2;
	using Direction = Interface::Direction;

	CSingle(const ISingle &from, const ISingle &to, const Predicate &predicate);
	~CSingle();

	void eval_int() const override;
};

}

#endif // CSINGLE_H
