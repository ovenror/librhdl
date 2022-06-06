/*
 * ls.c
 *
 *  Created on: May 13, 2021
 *      Author: ovenror
 */

#include <rhdl/rhdl.h>
#include "ls.h"
#include "macros.h"
#include "strlist.h"

static int checkLib(const char *const **list)
{
	CHECK(list, "AND");
	CHECK(list, "Clock3");
	CHECK(list, "ClockDiv2");
	CHECK(list, "D_Flipflop");
	CHECK(list, "Demul2");
	CHECK(list, "GRS_Latch");
	CHECK(list, "Inverter");
	CHECK(list, "NAND");
	CHECK(list, "NOR");
	CHECK(list, "Pulse1");
	CHECK(list, "RS_Latch");

	return SUCCESS;
}

int libOnly()
{
	rhdl_namespace_t *root = rhdl_namespace(0, 0);
	REQUIRE(root);

	const char *const *list = root -> entities;
	REQUIRE(list);

	REQUIRE(checkLib(&list));
	return REJECT(*list);
}

int libPlusNew()
{
	const char *new = "AA-Ron";
	rhdl_structure_t *s = rhdl_begin_structure(0, new, F_CREATE_STATELESS);
	REQUIRE(s);
	REQUIRE(s -> connector);

	rhdl_entity_t *i = rhdl_entity(0, "Inverter");
	REQUIRE(i);

	rhdl_connector_t *ic = rhdl_component(s, i);
	REQUIRE(ic);

	REQUIRE_NOERR(rhdl_connect(s -> connector, ic));
	REQUIRE_NOERR(rhdl_finish_structure(s));

	rhdl_namespace_t *root = rhdl_namespace(0, 0);
	REQUIRE(root);

	const char *const *list = root -> entities;
	REQUIRE(list);

	CHECK(&list, new);
	REQUIRE(checkLib(&list));
	return REJECT(*list);
}

int interface()
{
	rhdl_entity_t *and = rhdl_entity(0, "AND");
	REQUIRE(and);

	rhdl_iface_t *iface = and -> iface;
	REQUIRE(iface);
	REQUIRE(iface -> type == RHDL_COMPOSITE);

	const char *const *list = iface -> composite.interfaces;
	REQUIRE(list);

	CHECK(&list, "in0");
	CHECK(&list, "in1");
	CHECK(&list, "out");
	END(list);

	REQUIRE( ! rhdl_iface(iface, "uluasluh2397gh"));
	REQUIRE(rhdl_errno() == E_NO_SUCH_INTERFACE);

	rhdl_iface_t *out = rhdl_iface(iface, "out");
	REQUIRE(out);
	REQUIRE(out -> type == RHDL_SINGLE);

	const struct rhdl_isingle *single = &out -> single;
	return ACCEPT(single -> dir == RHDL_OUT && single -> open == 0);
}
