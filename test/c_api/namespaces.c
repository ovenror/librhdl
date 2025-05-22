/*
 * namespaces.c
 *
 *  Created on: Apr 17, 2025
 *      Author: js
 */

#include "namespaces.h"
#include "rhdl/rhdl.h"
#include "macros.h"

int noSuchNamespace()
{
	REQUIRE(!rhdl_namespace(0, "olololol"));
	REQUIRE_ERR(rhdl_errno(), E_NO_SUCH_MEMBER);

	return SUCCESS;
}

int createToplevelNamespace()
{
	rhdl_namespace_t *new = rhdl_create_namespace(0, "new_namespace");
	REQUIRE(new);

	new = rhdl_namespace(0, "new_namespace");
	REQUIRE(new);
	REQUIRE(new -> members);
	REQUIRE(!*new -> members);

	return SUCCESS;
}

int createNamespace()
{
	rhdl_namespace_t *entities = rhdl_namespace(0, "entities");
	REQUIRE(entities);

	rhdl_namespace_t *new = rhdl_create_namespace(entities, "new_namespace");
	REQUIRE(new);

	new = rhdl_namespace(entities, "new_namespace");
	REQUIRE(new);
	REQUIRE(new -> members);
	REQUIRE(!*new -> members);

	return SUCCESS;
}

int reCreateNamespace()
{
	rhdl_namespace_t *new = rhdl_create_namespace(0, "entities");
	REQUIRE(!new);
	REQUIRE_ERR(rhdl_errno(), E_MEMBER_EXISTS);

	return SUCCESS;
}

int createEntityInNamespace()
{
	rhdl_namespace_t *entities = rhdl_namespace(0, "entities");
	REQUIRE(entities);

	rhdl_namespace_t *ns = rhdl_create_namespace(entities, "new_namespace2");
	REQUIRE(ns);

	ns = rhdl_namespace(entities, "new_namespace2");
	REQUIRE(ns);

	rhdl_structure_t *s = rhdl_begin_structure(ns, "Test", F_CREATE_STATELESS);
	REQUIRE(s);

	rhdl_connector_t *sc = s -> connector;
	REQUIRE(sc);

	rhdl_entity_t *inverter = rhdl_entity(0, "Inverter");
	REQUIRE(inverter);

	rhdl_connector_t *inv = rhdl_component(s, inverter);
	REQUIRE(inv);

	REQUIRE_NOERR(rhdl_connect(sc, inv));
	REQUIRE_NOERR(rhdl_connect(inv, sc));
	REQUIRE_NOERR(rhdl_finish_structure(s));

	const char *const *list = ns -> members;
	REQUIRE(list);

	CHECK(&list, "Test");
	END(list);

	return SUCCESS;
}

