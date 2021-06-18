#ifndef CTRUE_H
#define CTRUE_H

#include "compatibilityresult.h"

namespace rhdl {

class CTrue : public CompatibilityResult
{
public:
	CTrue();
	~CTrue();

	void eval_int() const override;
	static bool compatible() {return true;}
};

}

#endif // CTRUE_H
