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
	DefaultTransformation() : TypedTransformation<FromRep, ToRep>(
			std::string("Default") + representations.objects()[FromRep::ID].name() + "_" +
					representations.objects()[ToRep::ID].name()) {}
	virtual ~DefaultTransformation() {}

	DefaultTransformation &cast() override {return *this;}

	std::unique_ptr<ToRep> execute(const FromRep &source) const override;
};

} /* namespace rhdl */

template<class FromRep, class ToRep>
inline std::unique_ptr<ToRep> rhdl::DefaultTransformation<FromRep, ToRep>::execute(
		const FromRep &source) const
{
	return ToRep::make(source);
}

#endif /* SRC_TRANSFORMATION_DEFAULTTRANSFORMATION_H_ */
