#include "timing.h"
#include "entity.h"

namespace rhdl {

Timing::Timing(const Entity &entity) :
	entity_(entity)
{
}

void Timing::add(const Representation &r)
{
	abiders_.push_back(&r);
}

bool Timing::operator==(const Timing &rhs) const
{
	return &rhs == this;
}

}
