/*
 * cpeerdirection.h
 *
 *  Created on: Nov 16, 2021
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_STRUCTURAL_BUILDER_CPEERDIRECTION_H_
#define SRC_REPRESENTATION_STRUCTURAL_BUILDER_CPEERDIRECTION_H_

#include "interface/cresult/compatibilityresult.h"
#include "interface/cresult/copdirection.h"

namespace rhdl::structural::builder {

class Port;

class CPeerDirection : public COpDirection {
public:
	CPeerDirection(const Port &peer, const ConnectionPredicate &predicate);
	virtual ~CPeerDirection();

	static bool compatible(const Port &, const ConnectionPredicate &);
};

} /* namespace rhdl */

#endif /* SRC_REPRESENTATION_STRUCTURAL_BUILDER_CPEERDIRECTION_H_ */
