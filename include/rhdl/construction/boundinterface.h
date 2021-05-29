#ifndef BOUNDINTERFACE_H
#define BOUNDINTERFACE_H

#include <rhdl/construction/connectible.h>

namespace rhdl {

class Interfacible;
class ConnectionPredicate;

class BoundInterface : public Connectible
{
public:
	BoundInterface(const Interface *iface);

	/*
	void operator>>(const Connectible &to) const;
	void operator<<(const Connectible &from) const;
	void operator>>(const BoundInterface &to) const;
	void operator<<(const BoundInterface &from) const;
	*/

	void operator=(const BoundInterface &to) const;

	std::pair<const Interface *, const Interface *> find_connectible_interfaces(const Connectible &to, const ConnectionPredicate &predicate) const override;
	std::pair<const Interface *, const Interface *> find_connectible_interfaces(const Interfacible &to, const ConnectionPredicate &predicate) const override;
	const Interface *find_connectible_interface(const Connectible &to, const ConnectionPredicate &predicate) const override;

	const Interface *interface() const override {return &iface_;}
	const Interfacible &interfacible() const override {return owner();}
	virtual const Interfacible &owner() const = 0;

	const Interface &iface_;
};

}

#endif // BOUNDINTERFACE_H
