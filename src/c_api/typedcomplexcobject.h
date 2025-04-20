/*
 * typedcomplexcobject.h
 *
 *  Created on: Apr 20, 2025
 *      Author: js
 */

#ifndef SRC_C_API_TYPEDCOMPLEXCOBJECT_H_
#define SRC_C_API_TYPEDCOMPLEXCOBJECT_H_

#include "typedcobject.h"

namespace rhdl {

template <class CRTP, class C_Struct, bool OWNING = true>
using TypedComplexCObject = TypedCObject<CRTP, C_Struct, OWNING, TypedCObjectBase::COMPLEX>;

} /* namespace rhdl */


#endif /* SRC_C_API_TYPEDCOMPLEXCOBJECT_H_ */
