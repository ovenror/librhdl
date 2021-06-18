#include "ccomposite.h"
#include "interface/icomposite.h"

namespace rhdl {

Errorcode CCompositeOps::fail_num(std::ostream &os) const
{
	os << "* different number of components\n";
	return Errorcode::E_DIFFERENT_NUMBER_OF_COMPONENTS;
}

Errorcode CCompositeOps::fail_component(std::ostream &os, size_t &idx, Interface::CResult sub) const
{
	os << "incompatible components at index " << idx
	   <<	":" << std::endl << sub -> error_msg();
	return sub -> error_code();
}

template <class RESULT>
void CCompositeOps::eval_common(RESULT &result) const {
	if (lhs.size() != rhs.size())
	{
		issue(&CCompositeOps::fail_num, result);
		return;
	}

	auto lhc = lhs.begin();
	auto rhc = rhs.begin();

	for (size_t index = 0; index < lhs.size(); ++index)
	{
		assert (lhc != lhs.end());
		assert (rhc != rhs.end());

		auto subresult = (*lhc) -> checkCompatTo<SubType<RESULT>>(**rhc, predicate);

		if (!checkSub(subresult)) {
			issue(&CCompositeOps::fail_component, result, index, smartmove(subresult));
			return;
		}

		++lhc;
		++rhc;
	}
}

template void CCompositeOps::eval_common(bool &) const;
template void CCompositeOps::eval_common(const CComposite &) const;

}
