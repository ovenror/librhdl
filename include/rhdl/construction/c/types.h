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

struct rhdl_isingle {
	enum rhdl_direction dir;
	int open;
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

typedef const struct rhdl_namespace_struct {
	const char *const *entities;
	//const char *const *nspaces;
} rhdl_namespace_t;



#endif /* INCLUDE_RHDL_CONSTRUCTION_C_TYPES_H_ */
