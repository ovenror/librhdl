/*
 * cvalue.h
 *
 *  Created on: Oct 3, 2024
 *      Author: ovenror
 */

#ifndef SRC_C_API_TYPEDCVALUE_H_
#define SRC_C_API_TYPEDCVALUE_H_

#include "cobjectimpl.h"
#include "cvalue.h"

#include <sstream>

namespace rhdl {

template <class ValueType>
struct CValueTypeToTypeID;

template <>
struct CValueTypeToTypeID<const char *> {
	constexpr static rhdl_type id = RHDL_CSTRING;
};

template <>
struct CValueTypeToTypeID<std::string> {
	constexpr static rhdl_type id = RHDL_CSTRING;
};

class Representation;

template <>
struct CValueTypeToTypeID<Cached<std::string, Representation> &> {
	constexpr static rhdl_type id = RHDL_CSTRING;
};

template <>
struct CValueTypeToTypeID<int64_t> {
	constexpr static rhdl_type id = RHDL_I64;
};

template <>
struct CValueTypeToTypeID<uint64_t> {
	constexpr static rhdl_type id = RHDL_U64;
};

template <>
struct CValueTypeToTypeID<rhdl_direction> {
	constexpr static rhdl_type id = RHDL_DIRECTION;
};

template <>
struct CValueTypeToTypeID<rhdl_reptype> {
	constexpr static rhdl_type id = RHDL_REPTYPE;
};

template <class VALUE_TYPE>
class TypedCValue :
		public CObjectImpl<
				TypedCValue<VALUE_TYPE>, VALUE_TYPE,
				CValueTypeToTypeID<VALUE_TYPE>::id, false, CValue> {

	using Super = CObjectImpl<
			TypedCValue<VALUE_TYPE>, VALUE_TYPE,
			CValueTypeToTypeID<VALUE_TYPE>::id,	false, CValue>;

public:
	using ValueType = VALUE_TYPE;

	TypedCValue(std::string name, const ValueType &value);
	TypedCValue(const TypedCValue &);
	TypedCValue(TypedCValue &&);
	virtual ~TypedCValue();

	TypedCValue &cast() override {return *this;}

	operator ValueType() const override {
		return value_;
	}

private:
	const char *to_cstring() const override;

	const ValueType &value_;
};

} /* namespace rhdl */


#endif /* SRC_C_API_TYPEDCVALUE_H_ */
