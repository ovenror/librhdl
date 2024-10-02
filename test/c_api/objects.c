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

int rootNamespace()
{
	rhdl_object_t *root = rhdl_get(0, 0);

	REQUIRE(root);
	REQUIRE(rhdl_get(root, 0) == root);

	const char *const *list = root -> members;

	REQUIRE(list);
	CHECK(&list, "entities");
	END(list);

	return SUCCESS;
}

int entities()
{
	rhdl_object_t *entities = rhdl_get(0, "entities");

	REQUIRE(entities);

	const char *const *list = entities -> members;
	REQUIRE(checkLib(&list));

	return SUCCESS;
}

int wrong()
{
	rhdl_object_t *bananas = rhdl_get(0, "bananas");

	REQUIRE(!bananas);
	REQUIRE_ERR(rhdl_errno(), E_NO_SUCH_MEMBER);

	return SUCCESS;
}
