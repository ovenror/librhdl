#ifndef CONNECTIBLEENTITY_H
#define CONNECTIBLEENTITY_H

#include <tuple>

namespace rhdl {

class Connectible;
class EntityHandle;
class Interfacible;
class ConnectionPredicate;

class Interface;

template <class BASE>
class ConnectibleEntity : public BASE
{
public:
	ConnectibleEntity();
	ConnectibleEntity(const Interface *interface);

	const Connectible &operator>>(const Connectible &to) const {return BASE::operator>>(to);}
	const Connectible &operator<<(const Connectible &from) const {return BASE::operator<<(from);}

	std::pair<const Interface *, const Interface *> find_connectible_interfaces(const Connectible &to, const ConnectionPredicate &predicate) const override;
	std::pair<const Interface *, const Interface *> find_connectible_interfaces(const Interfacible &to, const ConnectionPredicate &predicate) const override;

	const Interface *find_connectible_interface(const Connectible &to, const ConnectionPredicate &predicate) const override;

private:
	ConnectibleEntity &operator>>(const ConnectibleEntity &to);
	ConnectibleEntity &operator<<(const ConnectibleEntity &from);
};

}

#endif // CONNECTIBLEENTITY_H
