/*
 * simpleconnection.h
 *
 *  Created on: May 28, 2022
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_STRUCTURAL_BUILDER_SIMPLECONNECTION_H_
#define SRC_REPRESENTATION_STRUCTURAL_BUILDER_SIMPLECONNECTION_H_

#include "typedconnection.h"
#include "simpleport.h"

namespace rhdl {
namespace structural {
namespace builder {

class SimplePort;

class SimpleConnection : public TypedConnection<SimplePort> {
	using Super = TypedConnection<SimplePort>;

public:
	using Super::Super;
	virtual ~SimpleConnection();

	void build(ConnectionsBuilder &) const override;
};

} /* namespace builder */
} /* namespace structural */
} /* namespace rhdl */

#endif /* SRC_REPRESENTATION_STRUCTURAL_BUILDER_SIMPLECONNECTION_H_ */
