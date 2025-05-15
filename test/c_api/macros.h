/*
 * macros.h
 *
 *  Created on: May 10, 2021
 *      Author: ovenror
 */

#ifndef TEST_C_API_CMAKEFILES_MACROS_H_
#define TEST_C_API_CMAKEFILES_MACROS_H_

#include <stdio.h>
#include "utils.h"

#define FAIL 0
#define SUCCESS 1

#define ERRMSG(e, ...) do {if (e) printf(__VA_ARGS__)} while (0)

#define ACCEPT(b) (!! (b))
#define REJECT(b) (!(b))
#define REJECT_ERR(e) ((e) == E_NO_ERROR)

#define REQUIRE_MSG(b, ...) do {if (!(b)) {printf(__VA_ARGS__); return FAIL;}} while (0)
#define REQUIRE_NOERR_MSG(e, ...) do {if ((e) != E_NO_ERROR) {printf(__VA_ARGS__); return FAIL;}} while (0)

#define REQUIRE(b) REQUIRE_MSG(b, "REQUIRE failed in %s:%u\n", __FILE__, __LINE__)
#define REQUIRE_NOERR(e) REQUIRE_NOERR_MSG(e, "REQUIRE_NOERR failed in %s:%u\n", __FILE__, __LINE__)

#define REQUIRE_STREQ(a, b) REQUIRE(check_name(a, b))
#define CHECK(list, name) REQUIRE(check_list((list), (name)))
#define END(list) REQUIRE(!*list)

#define REQUIRE_ERR(e, exp) \
	do { \
		int e_ = (e); \
		int exp_ = (exp); \
		REQUIRE_MSG(e_ == exp_, "Expected error #%d, got #%d\n", exp_, e_); \
	} while(0)

#endif /* TEST_C_API_CMAKEFILES_MACROS_H_ */
