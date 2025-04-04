/*
 * cvalue.h
 *
 *  Created on: Oct 3, 2024
 *      Author: ovenror
 */

#ifndef SRC_C_API_TYPEDCVALUE_H_
#define SRC_C_API_TYPEDCVALUE_H_

#include "cvalue.h"
#include "cvaluetypetotypeid.h"
#include "stringconversion.h"
#include "util/cached.h"
#include "util/staticswitch.h"

#include <sstream>

namespace rhdl {

class CValueContainer;

template <class VALUE_TYPE>
class TypedCValue_Base : public CValue {
	using Super = CValue;

protected:
	static constexpr bool isCObjPtr = std::is_pointer<VALUE_TYPE>();
	using Pointee = typename std::remove_pointer_t<
			std::remove_cv_t<VALUE_TYPE>>;
	static constexpr bool pointeeIsTyped = !std::is_same<Pointee, CObject>();
	static constexpr bool isTypedCObjPtr = isCObjPtr && pointeeIsTyped;
	static constexpr bool isString = std::is_same<VALUE_TYPE, std::string>();

	using ReturnTypeNoStringRef = typename std::conditional_t<
			isTypedCObjPtr, const CObject *, VALUE_TYPE>;

	using ReturnType = typename std::conditional_t<
			isString, const std::string &, ReturnTypeNoStringRef>;

public:
	using ValueType = VALUE_TYPE;

	TypedCValue_Base(std::string name, CValueContainer &container);
	TypedCValue_Base(TypedCValue_Base &&, CValueContainer &newContainer);

	bool operator==(const TypedCValue_Base &other) const;

	virtual ~TypedCValue_Base() {}
};

template <class VALUE_TYPE, bool STORE_STRING>
class TypedCValue;

template <class VALUE_TYPE>
class TypedCValue<VALUE_TYPE, true>
		: public TypedCValue_Base<VALUE_TYPE> {
	using Super = TypedCValue_Base<VALUE_TYPE>;

public:
	TypedCValue(std::string name, CValueContainer &container);
	TypedCValue(TypedCValue &&, CValueContainer &newContainer);

	virtual ~TypedCValue() {}

	operator const std::string &() const override {return string_();}

private:
	virtual void createString(std::string &) const;

	Cached<std::string, TypedCValue> string_;
};

template <class VALUE_TYPE>
class TypedCValue<VALUE_TYPE, false> : public TypedCValue_Base<VALUE_TYPE>
{
	using Super = TypedCValue_Base<VALUE_TYPE>;

public:
	TypedCValue(std::string name, CValueContainer &container);
	TypedCValue(TypedCValue &&, CValueContainer &newContainer);

	virtual ~TypedCValue() {}
};

template <class VALUE_TYPE>
inline TypedCValue_Base<VALUE_TYPE>::TypedCValue_Base(
		std::string name, CValueContainer &container)
		: Super(CValueTypeToTypeID<ReturnType>::value, name, container) {}

template <class VALUE_TYPE>
inline TypedCValue_Base<VALUE_TYPE>::TypedCValue_Base(
		TypedCValue_Base<VALUE_TYPE> &&moved, CValueContainer &newContainer)
		: Super(std::move(moved), newContainer) {}

template <class VALUE_TYPE>
TypedCValue<VALUE_TYPE, false>::TypedCValue(
		std::string name, CValueContainer &container)
		: Super(name, container)
{}

template <class VALUE_TYPE>
TypedCValue<VALUE_TYPE, false>::TypedCValue(
		TypedCValue<VALUE_TYPE, false> &&moved, CValueContainer &newContainer)
		: Super(std::move(moved), newContainer)
{}

template <class VALUE_TYPE>
TypedCValue<VALUE_TYPE, true>::TypedCValue(
		std::string name, CValueContainer &container)
		: Super(name, container), string_(*this, &TypedCValue::createString)
{}

template <class VALUE_TYPE>
TypedCValue<VALUE_TYPE, true>::TypedCValue(
		TypedCValue &&moved, CValueContainer &newContainer)
		: Super(std::move(moved), newContainer), string_(*this, &TypedCValue::createString)
{}

template<class VALUE_TYPE>
inline void TypedCValue<VALUE_TYPE, true>::createString(std::string &str) const
{
	str = to_string(static_cast<typename Super::ReturnType>(*this));
}

template<>
inline void TypedCValue<std::string, true>::createString(std::string &str) const
{
	str = std::move(static_cast<std::string>(*this));
}

template<class VALUE_TYPE>
inline bool TypedCValue_Base<VALUE_TYPE>::operator ==(
		const TypedCValue_Base &other) const
{
	return static_cast<ReturnType>(*this) == static_cast<ReturnType>(other);
}

} /* namespace rhdl */

#endif /* SRC_C_API_TYPEDCVALUE_H_ */
