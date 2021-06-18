#ifndef SICOMPOSITE_H
#define SICOMPOSITE_H

#include <rhdl/construction/constructionexception.h>
#include "siminterface.h"
#include "interface/icomposite.h"
#include "siminterfacecontainer.h"
#include "sisingle_base.h"

namespace rhdl::behavioral {

template <class NAME, class... COMPONENTS>
class SIComposite : public SimInterface<IComposite, NAME> {

public:
	using Container = SimInterfaceContainer<COMPONENTS...>;
	using Super = SimInterface<IComposite, NAME>;

	SIComposite(const Interface *i) : Super(i), components_(Super::iface_ -> begin(), Super::iface_ -> end()) {}

	void toIMap(SISingle_Base::IMap &imap)
	{
		components_.toIMap(imap);
	}

	void record()
	{
		components_.record();
	}

	Container components_;
};

}

#endif // SICOMPOSITE_H
