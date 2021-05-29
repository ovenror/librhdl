#ifndef INTERFACECOMPATEXCEPTION_H
#define INTERFACECOMPATEXCEPTION_H

#include <rhdl/construction/constructionexception.h>
#include "interface/cresult/compatibilityresult.h"
#include <memory>

namespace rhdl {

class InterfaceCompatException : public ConstructionException
{
public:
	InterfaceCompatException(std::unique_ptr<CompatibilityResult> &&cresult);
	InterfaceCompatException(InterfaceCompatException &&);

	const char *what() const noexcept override;

	//const char *what() const noexcept override {return cresult_ -> error_msg().c_str();}
	//const char *what() const noexcept override {return msg_cpy_.c_str();}

private:
	InterfaceCompatException(const InterfaceCompatException &);

	std::unique_ptr<CompatibilityResult> cresult_;
};
}


#endif // INTERFACECOMPATEXCEPTION_H
