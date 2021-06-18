#ifndef COMPATIBILITY_H
#define COMPATIBILITY_H

#include <rhdl/construction/error.h>
#include <sstream>

namespace rhdl {

class CompatibilityResult
{
public:
	CompatibilityResult();
	virtual ~CompatibilityResult();

	bool success() const;
	std::string error_msg() const;
	Errorcode error_code() const;

	void eval() const;

protected:
	virtual void eval_int() const = 0;
	std::stringstream &incompatibility(Errorcode) const;
	void use(CompatibilityResult &&result) const;

	mutable bool evald_ = false;
	mutable bool success_ = true;
	mutable Errorcode ec_ = Errorcode::E_NO_ERROR;
	mutable std::stringstream msg_;
};

}

#endif // COMPATIBILITY_H
