#ifndef INTERFACIBLE_H
#define INTERFACIBLE_H

#include <rhdl/construction/connectible.h>

namespace rhdl {

class PartHandle;
class EntityHandle;
class ConnectionPredicate;

class Interfacible : public Connectible {
public:
	Interfacible();

	const Interfacible &interfacible() const override {return *this;}

	std::pair<const Interface *, const Interface *> find_connectible_interfaces(const Connectible &to, const ConnectionPredicate &predicate) const override;
	std::pair<const Interface *, const Interface *> find_connectible_interfaces(const Interfacible &to, const ConnectionPredicate &predicate) const override;

	const Interface *find_connectible_interface(const Connectible &to, const ConnectionPredicate &predicate) const override;

	//bool connect(Interface *iface, Interfacible &to, Interface *to_iface) {return false};
	virtual bool connect(const Interface *iface, const EntityHandle &to, const Interface *to_iface) const = 0;
	virtual bool connect(const Interface *iface, const PartHandle &to, const Interface *to_iface) const = 0;
	virtual bool get_connected(const Interface *iface, const Interfacible &from, const Interface *from_iface) const = 0;
};

}

#endif // CONNECTIBLE_H
