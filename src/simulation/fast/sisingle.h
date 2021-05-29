#ifndef SISINGLE_H
#define SISINGLE_H

#include "siminterface.h"
#include "sisingle_base.h"
#include "interface/isingle.h"

namespace rhdl {

template <class NAME>
class SISingle : public SISingle_Base, public SimInterface<ISingle, NAME>
{
public:
	using Super = SimInterface<ISingle, NAME>;
	SISingle(const Interface *i) : SISingle_Base(), Super(i) {}

	void toIMap(IMap &imap)
	{
		imap[Super::iface_] = this;
	}

	operator bool()
	{
		return state_;
	}

	SISingle operator=(bool vat) {
		state_ = vat;
		return *this;
	}

};

}


#endif // SISINGLE_H
