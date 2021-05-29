#ifndef CONNECTIBLE_H
#define CONNECTIBLE_H

#include <rhdl/construction/c/types.h>
#include <rhdl/construction/error.h>
#include <utility>

namespace rhdl {

class Interfacible;
class Interface;
class ConnectionPredicate;

class Connectible
{
public:
	Connectible();
	//virtual ~Connectible();

	const Connectible &operator>>(const Connectible &to) const;
	const Connectible &operator<<(const Connectible &from) const;

	virtual const Interface *interface() const = 0;
	virtual const Interfacible &interfacible() const = 0;

	rhdl::Errorcode try_connect(const Connectible &to, const ConnectionPredicate &predicate) const;

	virtual std::pair<const Interface *, const Interface *> find_connectible_interfaces(const Connectible &to, const ConnectionPredicate &predicate) const = 0;
	virtual std::pair<const Interface *, const Interface *> find_connectible_interfaces(const Interfacible &to, const ConnectionPredicate &predicate) const = 0;

	virtual const Interface *find_connectible_interface(const Connectible &to, const ConnectionPredicate &predicate) const = 0;
};

}

#endif // CONNECTIBLE_H
