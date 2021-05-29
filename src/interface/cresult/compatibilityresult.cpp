#include "compatibilityresult.h"
#include <iostream>

namespace rhdl {

CompatibilityResult::CompatibilityResult() :
	evald_(false), success_(false), ec_(Errorcode::E_INTERFACES_NOT_COMPATIBLE)
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

}
