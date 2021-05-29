/*
 * strlist.h
 *
 *  Created on: May 28, 2021
 *      Author: ovenror
 */

#ifndef TEST_C_API_STRLIST_H_
#define TEST_C_API_STRLIST_H_

#include <string.h>

#define CHECK(list, name) REQUIRE(check((list), (name)))
#define END(list) REQUIRE(!*list)

static int check(const char *const **list, const char *name)
{
	return !strncmp(*(*list)++, name, strlen(name) + 1);
}

#endif /* TEST_C_API_STRLIST_H_ */
