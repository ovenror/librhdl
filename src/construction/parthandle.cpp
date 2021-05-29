#include "library.h"
#include "parthandle.h"
#include "entityhandle.h"
#include "representation/structural/temporarynet.h"
#include "entity/entity.h"


namespace rhdl {

PartHandle::PartHandle(const std::string &name) : PartHandle(defaultLib.New(name))
{

}

PartHandle::PartHandle(const Entity *entity)
{
	TemporaryNet *newnet = new TemporaryNet();
	net_ = newnet;
	partidx_ = newnet -> add(entity, this);
}

PartHandle::PartHandle(PartHandle &&meh) :
	net_(meh.net_), partidx_(meh.partidx_)
{
}

PartHandle::~PartHandle()
{
}

PartInterface PartHandle::operator[](const std::string &iname) const
{
	//FIXME: what is the difference between ilookup, get and operator[]?
	return PartInterface(*this, entity() -> ilookup(iname));
}

bool PartHandle::connect(const Interface *iface, const PartHandle &to, const Interface *to_iface) const
{
	net_ -> absorb(to.net());
	return connect_local(iface, to, to_iface);
}

bool PartHandle::connect(const Interface *iface, const EntityHandle &to, const Interface *to_iface) const
{
	//reverse here!
	return to.connect(to_iface, *this, iface);
}

bool PartHandle::connect_local(const Interface *iface, const PartHandle &to, const Interface *to_iface) const
{
	return net_ -> connect(partidx_, iface, to.partidx_, to_iface);
}

bool PartHandle::get_connected(const Interface *iface, const Interfacible &from, const Interface *from_iface) const
{
	return from.connect(from_iface, *this, iface);
}

const Interface *PartHandle::interface() const
{
	return &(entity() -> interface());
}

const Entity *PartHandle::entity() const
{
	return net_ -> parts()[partidx_];
}

}
