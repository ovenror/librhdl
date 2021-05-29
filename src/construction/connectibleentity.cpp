#include <rhdl/construction/boundinterface.h>
#include <rhdl/construction/connectibleentity.h>
#include <rhdl/construction/interfacible.h>
#include "connectionpredicate.h"

namespace rhdl {

template <class BASE>
ConnectibleEntity<BASE>::ConnectibleEntity()
{
}
template ConnectibleEntity<Interfacible>::ConnectibleEntity();

template <class BASE>
ConnectibleEntity<BASE>::ConnectibleEntity(const Interface *interface) :
	BASE(interface)
{
}
template ConnectibleEntity<BoundInterface>::ConnectibleEntity(const Interface *interface);


template<class BASE>
std::pair<const Interface *, const Interface *> ConnectibleEntity<BASE>::find_connectible_interfaces(const Connectible &to, const ConnectionPredicate &predicate) const
{
	return BASE::find_connectible_interfaces(to, predicate.entity_corrected());
}

template std::pair<const Interface *, const Interface *> ConnectibleEntity<Interfacible>::find_connectible_interfaces(const Connectible &to, const ConnectionPredicate &predicate) const;
template std::pair<const Interface *, const Interface *> ConnectibleEntity<BoundInterface>::find_connectible_interfaces(const Connectible &to, const ConnectionPredicate &predicate) const;


template<class BASE>
std::pair<const Interface *, const Interface *> ConnectibleEntity<BASE>::find_connectible_interfaces(const Interfacible &to, const ConnectionPredicate &predicate) const
{
	return BASE::find_connectible_interfaces(to, predicate.entity_corrected());
}

template std::pair<const Interface *, const Interface *> ConnectibleEntity<Interfacible>::find_connectible_interfaces(const Interfacible &to, const ConnectionPredicate &predicate) const;
template std::pair<const Interface *, const Interface *> ConnectibleEntity<BoundInterface>::find_connectible_interfaces(const Interfacible &to, const ConnectionPredicate &predicate) const;

template<class BASE>
const Interface *ConnectibleEntity<BASE>::find_connectible_interface(const Connectible &to, const ConnectionPredicate &predicate) const
{
	return BASE::find_connectible_interface(to, predicate.entity_corrected());
}

template const Interface *ConnectibleEntity<Interfacible>::find_connectible_interface(const Connectible &to, const ConnectionPredicate &predicate) const;
template const Interface *ConnectibleEntity<BoundInterface>::find_connectible_interface(const Connectible &to, const ConnectionPredicate &predicate) const;



}
