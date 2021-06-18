#ifndef CCOMPOSITE_H
#define CCOMPOSITE_H

#include "cbase.h"

namespace rhdl {

class IComposite;

struct CCompositeOps : public CSametypeOpsBase<IComposite&, CCompositeOps> {
	using Super::Super;

	template <class RESULT>
	void eval_common(RESULT &result) const;

private:
	Errorcode fail_num(std::ostream &) const;

	Errorcode fail_component(std::ostream &, size_t &idx, Interface::CResult sub) const;
};

using CComposite = CBase<CCompositeOps>;

}

#endif // CCOMPOSITE_H
