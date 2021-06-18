#include "sisingle_base.h"

namespace rhdl::behavioral {

SISingle_Base::SISingle_Base()
	: state_(false), recent_()
{
}

void SISingle_Base::init_history(const std::vector<bool> &values)
{
	history_reserve(values.size() + 1);

	for (bool value : values)
		push(value);
}

void SISingle_Base::history_reserve(size_t capa)
{
	if (capa > recent_.capacity())
		recent_.set_capacity(capa);
}

}

