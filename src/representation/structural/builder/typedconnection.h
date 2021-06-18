/*
 * typedconnection.h
 *
 *  Created on: May 27, 2022
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_STRUCTURAL_BUILDER_TYPEDCONNECTION_H_
#define SRC_REPRESENTATION_STRUCTURAL_BUILDER_TYPEDCONNECTION_H_

#include "connection.h"
#include "../connectionbase.h"

#include "util/pointingpartitionclass.h"

namespace rhdl {
namespace structural {
namespace builder {

class ConnectionsBuilder;

template <class Port>
class TypedConnection : public ConnectionBase<PointingPartitionClass<Port>>, public Connection {
	using Super = ConnectionBase<PointingPartitionClass<Port>>;

public:
	TypedConnection(Port &p);
	TypedConnection(TypedConnection &&victim);

	virtual ~TypedConnection() {}

	void onAssign(ExistingPort &) {}
	void onReassign(ExistingPort &) {}
	void release(PartitionClassBase<Port> &);

	void connect(Port &);
};

template<class Port>
inline TypedConnection<Port>::TypedConnection(Port &p)
	: Super(p, *this)
{}

template<class Port>
inline TypedConnection<Port>::TypedConnection(TypedConnection &&victim)
	: Super(std::move(victim.container_))
{}

template<class Port>
inline void TypedConnection<Port>::release(
		PartitionClassBase<Port> &that)
{
	assert (&that == &(Super::container_));
	delete this;
}

template<class Port>
inline void TypedConnection<Port>::connect(Port &peer)
{
	if (connected(peer))
		return;

	auto &peerConnection = peer.connection();

	adoptRelativesFrom(peerConnection); //FIXME: use onIngest
	Super::container_.accept(peer);

	//assert (peerConnection.empty());
}

} /* namespace builder */
} /* namespace structural */
} /* namespace rhdl */

#endif /* SRC_REPRESENTATION_STRUCTURAL_BUILDER_TYPEDCONNECTION_H_ */
