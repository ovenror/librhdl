/*
 * object_classic.c
 *
 *  Created on: May 19, 2025
 *      Author: js
 */

#include <rhdl/rhdl.h>
#include "macros.h"

int entity()
{
	rhdl_object_t *entities = rhdlo_get(0, "entities");
	REQUIRE(entities);

	rhdl_object_t *inverter_o = rhdlo_get(entities, "Inverter");
	REQUIRE(inverter_o);

	rhdl_entity_t *inverter_c = rhdlo_entity(inverter_o);
	REQUIRE(inverter_c);
	REQUIRE(inverter_c -> iface);
	REQUIRE(inverter_c -> iface -> type == RHDL_COMPOSITE);

	const char *const *interfaces = inverter_c -> iface -> composite.interfaces;
	REQUIRE(interfaces);

	CHECK(&interfaces, "in");
	CHECK(&interfaces, "out");
	END(interfaces);
	return SUCCESS;
}

int no_entity()
{
	rhdl_object_t *entities = rhdlo_get(0, "entities");
	rhdl_entity_t *nope = rhdlo_entity(entities);
	REQUIRE(!nope);
	REQUIRE_ERR(rhdl_errno(), E_WRONG_OBJECT_TYPE);

	return SUCCESS;
}
