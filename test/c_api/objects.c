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
	rhdl_object_t *root = rhdlo_get(0, 0);

	REQUIRE(root);
	REQUIRE(rhdlo_get(root, 0) == root);
	REQUIRE(!rhdlo_has_value(root));

	const char *const *list = root -> members;

	REQUIRE(list);
	CHECK(&list, "entities");
	CHECK(&list, "transformations");
	END(list);

	return SUCCESS;
}

int entities()
{
	rhdl_object_t *entities = rhdlo_get(0, "entities");

	REQUIRE(entities);

	const char *const *list = entities -> members;
	REQUIRE(checkLib(&list));

	rhdl_object_t *none = rhdlo_get(entities, "bananananana");
	REQUIRE(!none);
	REQUIRE_ERR(rhdl_errno(), E_NO_SUCH_MEMBER);

	return SUCCESS;
}

int entity()
{
	rhdl_object_t *entities = rhdlo_get(0, "entities");
	rhdl_object_t *inverter = rhdlo_get(entities, "Inverter");

	REQUIRE(inverter);

	rhdl_object_t *iface = rhdlo_get(inverter, "interface");

	REQUIRE(iface);

	const char *const *list = iface -> members;

	REQUIRE(list);
	CHECK(&list, "in");
	CHECK(&list, "out");
	END(list);

	rhdl_object_t *in = rhdlo_get(iface, "in");
	rhdl_object_t *out = rhdlo_get(iface, "out");

	REQUIRE(in);
	REQUIRE(out);

	rhdl_object_t *direction_in = rhdlo_get(in, "direction");
	rhdl_object_t *direction_out = rhdlo_get(out, "direction");

	REQUIRE(direction_in);
	REQUIRE(direction_out);

	enum rhdl_direction in_value = rhdlo_read_direction(direction_in);
	enum rhdl_direction out_value = rhdlo_read_direction(direction_out);

	REQUIRE(in_value == RHDL_IN);
	REQUIRE(out_value == RHDL_OUT);

	rhdl_object_t *reps = rhdlo_get(inverter, "representations");
	REQUIRE(reps);

	rhdl_object_t *r = rhdlo_get(reps, "Inverter_Netlist_0");
	REQUIRE(r);

	list = r -> members;

	REQUIRE(list);
	CHECK(&list, "type");
	CHECK(&list, "parent");
	CHECK(&list, "content");
	END(list);

	rhdl_object_t *rtype = rhdlo_get(r, "type");
	REQUIRE(rtype);

	enum rhdl_reptype rt = rhdlo_read_reptype(rtype);
	REQUIRE(rt == RHDL_NETLIST);

	rhdl_object_t *rc = rhdlo_get(r, "content");
	REQUIRE(rc);

	printf("%s\n", rhdlo_read_cstring(rc));

	return SUCCESS;
}

int transformation()
{
	rhdl_object_t *entities = rhdlo_get(0, "transformations");
	rhdl_object_t *s2n = rhdlo_get(entities, "Structure2Netlist");

	REQUIRE(s2n);

	rhdl_object_t *srctype = rhdlo_get(s2n, "srctype");
	rhdl_object_t *dsttype = rhdlo_get(s2n, "dsttype");

	REQUIRE(srctype);
	REQUIRE(dsttype);

	enum rhdl_reptype srctype_value = rhdlo_read_reptype(srctype);
	enum rhdl_reptype dsttype_value = rhdlo_read_reptype(dsttype);

	REQUIRE(srctype_value == RHDL_STRUCTURE);
	REQUIRE(dsttype_value == RHDL_NETLIST);

	const char *srctype_str = rhdlo_read_cstring(srctype);
	const char *dsttype_str = rhdlo_read_cstring(dsttype);

	REQUIRE(srctype_str);
	REQUIRE(dsttype_str);
}

int wrong()
{
	rhdl_object_t *bananas = rhdlo_get(0, "bananas");

	REQUIRE(!bananas);
	REQUIRE_ERR(rhdl_errno(), E_NO_SUCH_MEMBER);

	return SUCCESS;
}
