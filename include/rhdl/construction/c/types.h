/*
 * types.h
 *
 *  Created on: May 18, 2021
 *      Author: ovenror
 */

#ifndef INCLUDE_RHDL_CONSTRUCTION_C_TYPES_H_
#define INCLUDE_RHDL_CONSTRUCTION_C_TYPES_H_

#include <stdint.h>

enum rhdl_direction {RHDL_INVALID_DIRECTION = -1, RHDL_IN = 0, RHDL_OUT = 1};
enum rhdl_iface_type {RHDL_SINGLE, RHDL_COMPOSITE, RHDL_UNSPECIFIED};

enum rhdl_type {RHDL_ENTITY, RHDL_REPTYPE, RHDL_REPRESENTATION,
		RHDL_TRANSFORMATION, RHDL_INTERFACE,
		RDHL_STRUCTURE, RHDL_OPTION, RHDL_NAMESPACE,
		RHDL_DIRECTION,
		RHDL_CSTRING, RHDL_I64, RHDL_U64,
		RHDL_OBJREF,
		RHDL_STRUCTURE_HANDLE, RHDL_CONNECTOR};

enum rhdl_reptype {RHDL_INVALID_REPTYPE = -1, RHDL_TIMEDBEHAVIOR,
		RHDL_FUNCTIONALBEHAVIOR, RHDL_STRUCTURE, RHDL_NETLIST, RHDL_TREE,
		RHDL_BLOCKS};
enum rhdl_member {RHDL_ENTITY_INTERFACE, RHDL_ENTITY_REPRESENTATIONS};

typedef const struct rhdl_object {
	enum rhdl_type type;
	const char *name;
	const struct rhdl_object *container;
	const char *const *members;
} rhdl_object_t;

typedef const struct rhdl_namespace_struct {
	const char *const *members;
} rhdl_namespace_t;

typedef const struct rhdl_representation {
	enum rhdl_reptype type;
	const struct rhdl_representation *parent;
} rhdl_representation_t;

typedef const struct rhdl_transformation {
	enum rhdl_reptype srctype;
	enum rhdl_reptype dsttype;
} rhdl_transformation_t;

struct rhdl_isingle {
	enum rhdl_direction dir;
};

struct rhdl_icomposite {
	const char *const *interfaces;
};

typedef const struct rhdl_iface_struct {
	enum rhdl_iface_type type;
	union {
		struct rhdl_isingle single;
		struct rhdl_icomposite composite;
	};
} rhdl_iface_t;

typedef const struct rhdl_connector {
	const rhdl_iface_t *iface;
} rhdl_connector_t;

typedef const struct rhdl_structure {
	const rhdl_connector_t *connector;
} rhdl_structure_t;

typedef const struct rhdl_entity_struct {
	const rhdl_iface_t *iface;
} rhdl_entity_t;




#endif /* INCLUDE_RHDL_CONSTRUCTION_C_TYPES_H_ */
