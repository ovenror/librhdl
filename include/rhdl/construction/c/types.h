/*
 * types.h
 *
 *  Created on: May 18, 2021
 *      Author: ovenror
 */

#ifndef INCLUDE_RHDL_CONSTRUCTION_C_TYPES_H_
#define INCLUDE_RHDL_CONSTRUCTION_C_TYPES_H_

enum rhdl_direction {RHDL_IN, RHDL_OUT};
enum rhdl_iface_type {RHDL_SINGLE, RHDL_COMPOSITE, RHDL_UNSPECIFIED};

enum rhdl_type {RHDL_ENTITY, RHDL_REPTYPE, RHDL_REPRESENTATION,
		RHDL_TRANSFORMATION, RHDL_INTERFACE, RHDL_CONNECTOR,
		RDHL_STRUCTURE, RHDL_OPTION, RHDL_NAMESPACE};

enum rhdl_reptype {STRUCURAL, NETLIST, BLOCKS};
enum rhdl_member {RHDL_ENTITY_INTERFACE, RHDL_ENTITY_REPRESENTATIONS};

typedef const struct rhdl_object {
	enum rhdl_type type;
	const char *const *members;
} rhdl_object_t;

typedef const struct rhdl_namespace_struct {
	const char *const *members;
} rhdl_namespace_t;

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
