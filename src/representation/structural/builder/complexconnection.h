/*
 * complexconnection.h
 *
 *  Created on: May 27, 2022
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_STRUCTURAL_BUILDER_COMPLEXCONNECTION_H_
#define SRC_REPRESENTATION_STRUCTURAL_BUILDER_COMPLEXCONNECTION_H_

#include "typedconnection.h"
#include "complexport.h"

namespace rhdl {
namespace structural {
namespace builder {

class ComplexPort;

class ComplexConnection : public TypedConnection<ComplexPort> {
	using Super = TypedConnection<ComplexPort>;

public:
	using Super::Super;

	void setInitialChildren(std::set<Connection *>);

	void adoptRelativesFrom(Connection &) override;
	void adoptRelativesFrom(ComplexConnection &);
	void getRelativesAdoptedBy(ComplexConnection &) override;

	void build(ConnectionsBuilder &) const override;

	void replaceChild(Connection &, Connection &);

	const std::set<Connection *> &children() const {return children_;}

private:
	std::set<Connection *> children_;
};

} /* namespace builder */
} /* namespace structural */
} /* namespace rhdl */

#endif /* SRC_REPRESENTATION_STRUCTURAL_BUILDER_COMPLEXCONNECTION_H_ */
