#ifndef CBASE_H
#define CBASE_H

#include "compatibilityresult.h"
#include "interface/predicate.h"

namespace rhdl {

template<class IFACE>
class CBase : public CompatibilityResult
{
public:
	using Predicate = Interface::Predicate2;

	CBase(const IFACE &from, const IFACE &to, const Predicate &predicate);
	~CBase();

protected:
	const IFACE &from_;
	const IFACE &to_;
	const Predicate predicate_;
};

}

#endif // CBASE_H
