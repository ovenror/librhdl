/*
 * objects.c
 *
 *  Created on: Sep 28, 2024
 *      Author: ovenror
 */

#include <rhdl/rhdl.h>
#include "objects.h"
#include "macros.h"
#include "strlist.h"
#include "utils.h"

#include <stdio.h>

int rootNamespace()
{
	rhdl_object_t *root = rhdl_get(0, 0);

	REQUIRE(root);
	REQUIRE(rhdl_get(root, 0) == root);
	REQUIRE(!rhdl_has_value(root));

	const char *const *list = root -> members;

	REQUIRE(list);
	CHECK(&list, "entities");
	CHECK(&list, "transformations");
	END(list);

	return SUCCESS;
}

int entities()
{
	rhdl_object_t *entities = rhdl_get(0, "entities");

	REQUIRE(entities);

	const char *const *list = entities -> members;
	REQUIRE(checkLib(&list));

	rhdl_object_t *none = rhdl_get(entities, "bananananana");
	REQUIRE(!none);
	REQUIRE_ERR(rhdl_errno(), E_NO_SUCH_ENTITY);

	return SUCCESS;
}

int entity()
{
	rhdl_object_t *entities = rhdl_get(0, "entities");
	rhdl_object_t *inverter = rhdl_get(entities, "Inverter");

	REQUIRE(inverter);

	rhdl_object_t *iface = rhdl_get(inverter, "interface");

	REQUIRE(iface);

	rhdl_object_t *in = rhdl_get(iface, "in");
	rhdl_object_t *out = rhdl_get(iface, "out");

	REQUIRE(in);
	REQUIRE(out);

	rhdl_object_t *direction_in = rhdl_get(in, "direction");
	rhdl_object_t *direction_out = rhdl_get(out, "direction");

	REQUIRE(direction_in);
	REQUIRE(direction_out);

	enum rhdl_direction in_value = rhdl_read_direction(direction_in);
	enum rhdl_direction out_value = rhdl_read_direction(direction_out);

	REQUIRE(in_value == RHDL_IN);
	REQUIRE(out_value == RHDL_OUT);

	rhdl_object_t *reps = rhdl_get(inverter, "representations");
	REQUIRE(reps);

	rhdl_object_t *r = rhdl_get(reps, "Inverter_Netlist_0");
	REQUIRE(r);

	const char *const *list = r -> members;

	/*
	REQUIRE(list);
	CHECK(&list, "content");
	CHECK(&list, "type");
	END(list);
*/
	rhdl_object_t *rtype = rhdl_get(r, "type");
	REQUIRE(rtype);

	enum rhdl_reptype rt = rhdl_read_reptype(rtype);
	REQUIRE(rt == RHDL_NETLIST);

	rhdl_object_t *rc = rhdl_get(r, "content");
	REQUIRE(rc);

	printf("%s\n", rhdl_read_cstring(rc));

	return SUCCESS;
}

int wrong()
{
	rhdl_object_t *bananas = rhdl_get(0, "bananas");

	REQUIRE(!bananas);
	REQUIRE_ERR(rhdl_errno(), E_NO_SUCH_MEMBER);

	return SUCCESS;
}
