#ifndef SIMINTERFACECONTAINER_H
#define SIMINTERFACECONTAINER_H

#include "interface/icomposite.h"
#include "sisingle_base.h"

namespace rhdl {

template <class... COMPONENTS>
class SimInterfaceContainer {
public:
	using CIter = IComposite::InterfaceContainer::const_iterator;

	SimInterfaceContainer(CIter next, CIter end) {
		assert(next == end);
	}

	void toIMap(SISingle_Base::IMap &imap) {std::ignore = imap;}
	void record() {}
};

template <class COMPONENT0, class... COMPONENTS>
class SimInterfaceContainer<COMPONENT0, COMPONENTS...> {
public:
	using CIter = IComposite::InterfaceContainer::const_iterator;

	SimInterfaceContainer(CIter next, CIter end) : first_(*next), rest_(next+1, end) {}

	void toIMap(SISingle_Base::IMap &imap)
	{
		first_.toIMap(imap);
		rest_.toIMap(imap);
	}

	void record()
	{
		first_.record();
		rest_.record();
	}

	COMPONENT0 first_;
	SimInterfaceContainer<COMPONENTS...> rest_;
};

}


#endif // SIMINTERFACECONTAINER_H
