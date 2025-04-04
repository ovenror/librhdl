/*
 * ctypetotypeid.h
 *
 *  Created on: Apr 10, 2025
 *      Author: js
 */

#ifndef SRC_C_API_CVALUETYPETOTYPEID_H_
#define SRC_C_API_CVALUETYPETOTYPEID_H_

#include <rhdl/construction/c/types.h>

namespace rhdl {

class CObject;

template <class ValueType>
struct CValueTypeToTypeID;

template <>
struct CValueTypeToTypeID<const CObject *> {
	constexpr static rhdl_type value = RHDL_OBJREF;
};

template <>
struct CValueTypeToTypeID<const char *> {
	constexpr static rhdl_type value = RHDL_CSTRING;
};

template <>
struct CValueTypeToTypeID<const std::string &> {
	constexpr static rhdl_type value = RHDL_CSTRING;
};

template <>
struct CValueTypeToTypeID<int64_t> {
	constexpr static rhdl_type value = RHDL_I64;
};

template <>
struct CValueTypeToTypeID<uint64_t> {
	constexpr static rhdl_type value = RHDL_U64;
};

template <>
struct CValueTypeToTypeID<rhdl_direction> {
	constexpr static rhdl_type value = RHDL_DIRECTION;
};

template <>
struct CValueTypeToTypeID<rhdl_reptype> {
	constexpr static rhdl_type value = RHDL_REPTYPE;
};

}

#endif /* SRC_C_API_CVALUETYPETOTYPEID_H_ */
