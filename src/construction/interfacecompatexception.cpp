#include "interfacecompatexception.h"
#include <iostream>
#include <cassert>
#include <sstream>

namespace rhdl {

InterfaceCompatException::InterfaceCompatException(std::unique_ptr<CompatibilityResult> &&cresult) :
	ConstructionException(cresult -> error_code()), cresult_(std::forward<std::unique_ptr<CompatibilityResult> >(cresult))
{
	cresult_ -> eval();
	//std::cerr << "ICE constructor w/ result @" << cresult_.get() << ": " << cresult_ -> error_msg() << std::endl;

	std::stringstream strm;
	strm << "Interfaces are not compatible: " << std::endl << cresult_ -> error_msg();
	info_ = strm.str();
}

InterfaceCompatException::InterfaceCompatException(InterfaceCompatException &&e) :
	InterfaceCompatException(std::move(e.cresult_))
{
	//std::cerr << "ICE && constructor w/ result @" << cresult_.get() << ": " << cresult_ -> error_msg() << std::endl;
}

const char *InterfaceCompatException::what() const noexcept
{
	assert (cresult_);
	return info_.c_str();
}

}

