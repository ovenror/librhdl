/*
 * cinclude.h
 *
 *  Created on: May 10, 2021
 *      Author: ovenror
 */

#ifndef INCLUDE_CONSTRUCTION_FUNCTIONS_H_
#define INCLUDE_CONSTRUCTION_FUNCTIONS_H_

int rhdl_errno();
const char *rhdl_errstr();

const rhdl_namespace_t *rhdl_namespace(const rhdl_namespace_t* ns, const char *name);
const rhdl_entity_t *rhdl_entity(const rhdl_namespace_t* ns, const char *name);
const rhdl_iface_t *rhdl_iface(const rhdl_iface_t *iface, const char *name);

const rhdl_structure_t *rhdl_begin_structure(const rhdl_namespace_t *nspace, const char *entity_name, unsigned int mode);
const rhdl_connector_t *rhdl_component(const rhdl_structure_t *structure, const rhdl_entity_t *entity);
const rhdl_connector_t *rhdl_select(const rhdl_connector_t *connector, const char *iface_name);
int rhdl_connect(const rhdl_connector_t *from, const rhdl_connector_t *to);
int rhdl_finish_structure(const rhdl_structure_t *structure);

int rhdl_print_commands(const char *entity_name);

#endif /* INCLUDE_CONSTRUCTION_FUNCTIONS_H_ */
