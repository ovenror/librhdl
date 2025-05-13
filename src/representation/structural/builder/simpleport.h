/*
 * simpleport.h
 *
 *  Created on: Nov 17, 2021
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_STRUCTURAL_BUILDER_SIMPLEPORT_H_
#define SRC_REPRESENTATION_STRUCTURAL_BUILDER_SIMPLEPORT_H_

#include <representation/structural/builder/existingportbase.h>
#include "interface/isingle.h"

namespace rhdl {
namespace structural {
namespace builder {

class SimplePort: public ExistingPortBase<SimplePort> {
public:
	SimplePort(Element &, const ISingle &);
	SimplePort(Element &, const ISingle &, WPtr &&);
	virtual ~SimplePort();

	virtual void connectCompat(ExistingPort &peer) override;
	virtual void connectCompat(SimplePort &peer) override;

	const ISingle &iface() const override {return iface_;}

private:
	void constructorCommon();

	const ISingle &iface_;
};

} /* namespace builder */
} /* namespace structural */
} /* namespace rhdl */

#endif /* SRC_REPRESENTATION_STRUCTURAL_BUILDER_SIMPLEPORT_H_ */
