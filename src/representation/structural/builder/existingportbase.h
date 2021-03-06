/*
 * typedexistingport.h
 *
 *  Created on: May 27, 2022
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_STRUCTURAL_BUILDER_EXISTINGPORTBASE_H_
#define SRC_REPRESENTATION_STRUCTURAL_BUILDER_EXISTINGPORTBASE_H_

#include "existingport.h"
#include "util/partitionable.h"

namespace rhdl {
namespace structural {
namespace builder {


template <class DERIVED>
class ExistingPortBase : public Partitionable<DERIVED, TypedConnection<DERIVED>>, public ExistingPort {
	using PartitionableBase = Partitionable<DERIVED, TypedConnection<DERIVED>>;

public:
	using ExistingPort::ExistingPort;
	virtual ~ExistingPortBase() {}

	void connectCompat(DERIVED &peer);

	Connection &connection() const override;
};

template<class DERIVED>
inline void ExistingPortBase<DERIVED>::connectCompat(DERIVED &peer)
{
	assert(PartitionableBase::pcOwner());
	PartitionableBase::pcOwner() -> connect(peer);
}

template<class DERIVED>
inline Connection &ExistingPortBase<DERIVED>::connection() const
{
	assert(PartitionableBase::pcOwner());
	return *PartitionableBase::pcOwner();
}

} /* namespace builder */
} /* namespace structural */
} /* namespace rhdl */

#endif /* SRC_REPRESENTATION_STRUCTURAL_BUILDER_EXISTINGPORTBASE_H_ */
