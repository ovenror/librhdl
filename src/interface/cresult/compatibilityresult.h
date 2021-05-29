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

	mutable bool evald_;
	mutable bool success_;
	mutable Errorcode ec_;
	mutable std::stringstream msg_;
};

}

#endif // COMPATIBILITY_H
