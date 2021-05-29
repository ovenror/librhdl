#include "entityhandle.h"
#include "construction/library.h"
#include "entity/entity.h"
#include "entity/timing.h"
#include "representation/structural/structural.h"
#include "interface/iplaceholder.h"

namespace rhdl {

EntityHandle::EntityHandle(const Entity &entity)
	: timing_(&entity.addTiming()),
	  structure_(std::make_unique<Structural>(entity, nullptr, timing_)),
	  finalized_(false)
{}

EntityInterface EntityHandle::operator[](const std::string &iname) const
{
	return EntityInterface(*this, (*interface())[iname]);
}

bool EntityHandle::connect(const Interface *iface, const PartHandle &to, const Interface *to_iface) const
{
	net().absorb(to.net());

	return structure_ -> expose(to.partidx_, to_iface, IPlaceholder::get_final(iface));
}

bool EntityHandle::connect(const Interface *iface, const EntityHandle &to, const Interface *to_iface) const
{
	std::ignore = iface;
	std::ignore = to;
	std::ignore = to_iface;
	return false;
}

bool EntityHandle::get_connected(const Interface *iface, const Interfacible &from, const Interface *from_iface) const
{
	return from.connect(from_iface, *this, iface);
}

Net &EntityHandle::net() const
{
	return *structure_;
}

bool EntityHandle::finalizeOnce() {
	if (finalized_)
		return false;

	finalized_ = true;
	entity().addRepresentation(std::move(structure_));
	return true;
}

}

