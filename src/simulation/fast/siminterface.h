#ifndef SIMINTERFACE_H
#define SIMINTERFACE_H

#include <rhdl/construction/constructionexception.h>
#include <cassert>
#include <map>
#include "interface/interface.h"

namespace rhdl {

class Entity;

namespace behavioral {

template <class WHAT, class NAME>
class SimInterface
{
public:
	SimInterface(const Interface *i)
	{
		auto chrs = NAME::chars();
		std::string static_name = chrs.data();

		if (static_name != i -> name())
			throw ConstructionException(Errorcode::E_WRONG_NAME, std::string("is '" + static_name + ", should be " + i -> name()));

		iface_ = dynamic_cast<const WHAT *>(i);

		if (!iface_)
			throw ConstructionException(Errorcode::E_WRONG_TYPE);
	}

	const WHAT *iface_;
};

}}

#endif // SIMINTERFACE_H
