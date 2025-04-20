/*
 * ValueObject.h
 *
 *  Created on: Oct 13, 2024
 *      Author: js
 */

#ifndef SRC_C_API_TYPEDVALUEOBJECT_H_
#define SRC_C_API_TYPEDVALUEOBJECT_H_

#include "typedcobject.h"

namespace rhdl {

template <class CRTP, class C_Struct>
using TypedValueObject = TypedCObject<CRTP, C_Struct, false, TypedCObjectBase::VALUES_ONLY>;

} /* namespace rhdl */

#endif /* SRC_C_API_TYPEDVALUEOBJECT_H_ */
