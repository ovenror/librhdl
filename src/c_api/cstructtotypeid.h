/*
 * cstructtotypeid.h
 *
 *  Created on: Apr 11, 2025
 *      Author: js
 */

#ifndef SRC_C_API_CSTRUCTTOTYPEID_H_
#define SRC_C_API_CSTRUCTTOTYPEID_H_

#include <rhdl/construction/c/types.h>

template <class ValueType>
struct CStructToTypeID;

template <>
struct CStructToTypeID<rhdl_iface_struct> {
	constexpr static rhdl_type value = RHDL_INTERFACE;
};

template <>
struct CStructToTypeID<rhdl_entity_struct> {
	constexpr static rhdl_type value = RHDL_ENTITY;
};

template <>
struct CStructToTypeID<rhdl_namespace_struct> {
	constexpr static rhdl_type value = RHDL_NAMESPACE;
};

template <>
struct CStructToTypeID<rhdl_representation> {
	constexpr static rhdl_type value = RHDL_REPRESENTATION;
};

template <>
struct CStructToTypeID<rhdl_transformation> {
	constexpr static rhdl_type value = RHDL_TRANSFORMATION;
};

template <>
struct CStructToTypeID<rhdl_structure> {
	constexpr static rhdl_type value = RHDL_STRUCTURE_HANDLE;
};

#endif /* SRC_C_API_CSTRUCTTOTYPEID_H_ */
