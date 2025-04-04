/*
 * cobjectreference.h
 *
 *  Created on: Apr 12, 2025
 *      Author: js
 */

#ifndef SRC_C_API_COBJECTREFERENCE_H_
#define SRC_C_API_COBJECTREFERENCE_H_

#include <c_api/referencedcvalue.h>
#include "dynamiccvalue.h"
#include "cacheddynamiccvalue.h"

namespace rhdl {

template <template <class> class PROVISION, class VALUE_TYPE>
class CObjectReferenceWrapper;



template <template <class> class PROVISION, class VALUE_TYPE>
class CObjectReference {
	static_assert(0, "2nd template parameter must be * or &");
};

template <template <class> class PROVISION>
class CObjectReference<PROVISION, CObject *> : public PROVISION<CObject *> {};
template <template <class> class PROVISION>
class CObjectReference<PROVISION, const CObject *> : public PROVISION<const CObject *> {};
template <template <class> class PROVISION>
class CObjectReference<PROVISION, CObject &> : public PROVISION<CObject &> {};
template <template <class> class PROVISION>
class CObjectReference<PROVISION, const CObject &> : public PROVISION<const CObject &> {};

template <template <class> class PROVISION, class REFEREE>
class CObjectReference<PROVISION, REFEREE *> : public PROVISION<REFEREE *>
{
	using Super = PROVISION<REFEREE *>;

public:
	template<class... Args>
	CObjectReference(Args&&... args) : Super(std::forward<Args>(args)...) {}

	operator REFEREE *() const {return Super::value();}
	REFEREE * operator->() const {return Super::value();}
	operator bool() const {return Super::value();}
};

template <template <class> class PROVISION, class REFEREE>
class CObjectReference<PROVISION, REFEREE &> : public PROVISION<REFEREE *>
{
	using Super = PROVISION<REFEREE *>;

public:
	template<class... Args>
	CObjectReference(Args&&... args) : Super(std::forward<Args>(args)...) {}

	const CObject &getRef() const override {return *Super::value();}
	operator REFEREE &() const {return *Super::value();}
};

template <class VALUE_TYPE>
using StoredCObjectReference = CObjectReference<StoredCValue, VALUE_TYPE>;

}

#endif /* SRC_C_API_COBJECTREFERENCE_H_ */
