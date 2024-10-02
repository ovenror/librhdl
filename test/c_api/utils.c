/*
 * utils.c
 *
 *  Created on: Oct 2, 2024
 *      Author: ovenror
 */

#include "utils.h"
#include "macros.h"
#include "strlist.h"

int check(const char *const **list, const char *name)
{
	return !strncmp(*(*list)++, name, strlen(name) + 1);
}

int checkLib(const char *const **list)
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

