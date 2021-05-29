#ifndef CTRUE_H
#define CTRUE_H

#include "compatibilityresult.h"

namespace rhdl {

class CTrue : public CompatibilityResult
{
public:
	CTrue();
	~CTrue();

	void eval_int() const override {
		success_ = true;
		ec_ = Errorcode::E_NO_ERROR;
	}
};

}

#endif // CTRUE_H
