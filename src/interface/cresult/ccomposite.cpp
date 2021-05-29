#include "ccomposite.h"
#include "interface/icomposite.h"

namespace rhdl {


CComposite::CComposite(const IComposite &from, const IComposite &to, const Predicate &predicate) :
	BASE(from, to, predicate)
{
}

CComposite::~CComposite()
{

}

void CComposite::eval_int() const
{
	size_t num_components = from_.components_.size();

	if (num_components != to_.components_.size ())
	{
		msg_ << "* different number of components\n";
		ec_ = Errorcode::E_DIFFERENT_NUMBER_OF_COMPONENTS;
		success_ = false;
		return;
	}

	for (size_t index = 0; index < num_components; ++index)
	{
		Interface::CResult subresult(from_.components_ [index] -> eq_struct_int (*to_.components_ [index], predicate_));

		if (!subresult -> success())
		{
			msg_ << std::string("incompatible components at index ") << index;
			msg_ << ":" << std::endl;
			msg_ << subresult -> error_msg();
			ec_ = subresult -> error_code();
			success_ = false;
			return;
		}
	}

	ec_ = Errorcode::E_NO_ERROR;
	success_ = true;
}

}
