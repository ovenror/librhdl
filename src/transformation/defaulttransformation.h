/*
 * defauilttransformation.h
 *
 *  Created on: Jun 23, 2021
 *      Author: ovenror
 */

#ifndef SRC_TRANSFORMATION_DEFAULTTRANSFORMATION_H_
#define SRC_TRANSFORMATION_DEFAULTTRANSFORMATION_H_

#include "transformation/typedtransformation.h"

namespace rhdl {

template <class FromRep, class ToRep>
class DefaultTransformation : public TypedTransformation<FromRep, ToRep> {
public:
	DefaultTransformation() {}
	virtual ~DefaultTransformation() {}

	ToRep execute(const FromRep &source) const override {return ToRep(source);}
};

} /* namespace rhdl */

#endif /* SRC_TRANSFORMATION_DEFAULTTRANSFORMATION_H_ */
