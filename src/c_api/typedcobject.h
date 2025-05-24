/*
 * typedcobject.h
 *
 *  Created on: Sep 29, 2024
 *      Author: ovenror
 */

#ifndef SRC_C_API_TYPEDCOBJECT_H_
#define SRC_C_API_TYPEDCOBJECT_H_

#include <c_api/complexcobject.h>
#include "cvalueobject.h"
#include "cstructtotypeid.h"

#include "util/staticswitch.h"
#include "util/dictionary/mutabledictionary.h"

#include <cassert>
#include <iostream>

namespace rhdl {

enum class TypedCObjectBase {BASIC, COMPLEX, VALUES_ONLY};

template <bool OWNING, TypedCObjectBase BASE>
class MakeTypedCObjectBase {
	using Setup = staticswitch::Setup<TypedCObjectBase, staticswitch::Class>;

	template <TypedCObjectBase SWITCH, class DEFAULT, class... CASES>
	using Switch = Setup::Switch<SWITCH, DEFAULT, CASES...>;

	template <TypedCObjectBase CASE, class RESULT>
	using Case = Setup::Case<CASE, RESULT>;

public:
	using type =
			Switch<BASE, CObject,
				Case<TypedCObjectBase::COMPLEX, ComplexCObject<OWNING>>,
				Case<TypedCObjectBase::VALUES_ONLY, CValueObject>>;
};

template <
		class CRTP, class Typed_C_Struct,
		bool OWNING = true, TypedCObjectBase BASE = TypedCObjectBase::BASIC>
class TypedCObject : public MakeTypedCObjectBase<OWNING, BASE>::type
{
	static constexpr rhdl_type typeId = CStructToTypeID<Typed_C_Struct>::value;

public:
	using Super = typename MakeTypedCObjectBase<OWNING, BASE>::type;
	using C_Struct = Typed_C_Struct;

	TypedCObject(std::string name) : Super(typeId, name), c_(*this) {}

	//template <class DICT, std::enable_if_t<isComplex<DICT>, bool> dummy = true>
	template <class DICT>
	TypedCObject(
			std::string name,
			std::unique_ptr<DICT> dict)
			: Super(typeId, name, std::move(dict)), c_(*this)
	{
		static_assert(BASE != TypedCObjectBase::BASIC);
	}

	TypedCObject(TypedCObject &&moved)
			: Super(std::move(moved)), c_(std::move(moved.c_), *this) {}

	virtual ~TypedCObject() {}

	operator CRTP &() {return cast();}
	operator const CRTP &() const {return cast();}

	static CRTP &recover(const C_Struct *c) {
		TypedCObject &tco = rhdl::recover<TypedCObject>(c);
		return tco.cast();
	}

	const C_Struct *c_ptr() const {return rhdl::c_ptr(*this);}

protected:
	C_Struct *c_ptr() {return rhdl::c_ptr(*this);}
	virtual CRTP &cast() = 0;
	const CRTP &cast() const {return const_cast<TypedCObject &>(*this).cast();}

private:
	virtual void setTypedMembers(const char *const *) {}
	void setTypedMembers() {setTypedMembers(Super::c_strings().data());}

	void setMembers() override
	{
		Super::setMembers();
		setTypedMembers();
	}

protected:
	friend class Wrapper<TypedCObject>;
	friend class Wrapper<CRTP>;

	static constexpr unsigned long C_ID = 0x7193D0813C7 + typeId;
	Wrapper<TypedCObject> c_;
};

}

#endif /* SRC_C_API_TYPEDCOBJECT_H_ */
