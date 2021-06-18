#include "compatibilityresult.h"
#include <iostream>

namespace rhdl {

CompatibilityResult::CompatibilityResult()
{
	//std::cerr << "  GET CONSTRUED! @" << this << std::endl;
}

CompatibilityResult::~CompatibilityResult()
{
	//std::cerr << "  GET DESTROYED! @" << this << std::endl;
}

bool CompatibilityResult::success() const
{
	eval();
	return success_;
}

std::string CompatibilityResult::error_msg() const
{
	eval();
	return msg_.str();
}

Errorcode CompatibilityResult::error_code() const
{
	eval();
	return ec_;
}

void CompatibilityResult::eval() const
{
	if (evald_)
		return;

	eval_int();
	evald_ = true;
}

std::stringstream& CompatibilityResult::incompatibility(Errorcode errorcode) const
{
	ec_ = errorcode;
	success_ = false;
	return msg_;
}

void CompatibilityResult::use(CompatibilityResult &&result) const
{
	result.eval();

	success_ = result.success_;
	ec_ = result.ec_;
	msg_ = std::move(result.msg_);
}

}
