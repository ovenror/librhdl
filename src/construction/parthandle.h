#ifndef PARTHANDLE_H
#define PARTHANDLE_H

#include <rhdl/construction/interfacible.h>
#include <rhdl/construction/partinterface.h>
#include "representation/structural/net.h"

#include <utility>
#include <string>
#include <memory>

namespace rhdl {

class Entity;
class Net;
class PartHandle;
class Interface;
class ISingle;

class PartHandle : public Interfacible
{
public:
	PartHandle(const std::string &name);

	PartHandle(const Entity *entity);
	PartHandle(PartHandle &&meh);
	~PartHandle();

	PartInterface operator[] (const std::string &iname) const;
	PartInterface operator[] (const Interface *iface);

	bool connect(const Interface *iface, const PartHandle &to, const Interface *to_iface) const override;
	bool connect(const Interface *iface, const EntityHandle &to, const Interface *to_iface) const override;
	bool get_connected(const Interface *iface, const Interfacible &from, const Interface *from_iface) const override;

	bool connect_local(const Interface *iface, const PartHandle &to, const Interface *to_iface) const;

	void update_after_meld(PartIdx offset, Net *newnet) {partidx_ += offset; net_ = newnet;}

	const Interface *interface() const override;
	Net &net() const {return *net_;}

	const Entity *entity() const;

	void migrate_to_net(Net *newnet) const;

	//std::shared_ptr<Net> net_;
	mutable Net *net_;
	PartIdx partidx_;

private:
	PartHandle(const PartHandle &);
	PartHandle &operator=(const PartHandle &);

};

}
#endif // PART_H
