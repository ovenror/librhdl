/*
 * macros.h
 *
 *  Created on: May 10, 2021
 *      Author: ovenror
 */

#ifndef TEST_C_API_CMAKEFILES_MACROS_H_
#define TEST_C_API_CMAKEFILES_MACROS_H_

#define FAIL 0
#define SUCCESS 1

#define ACCEPT(b) (!! (b))
#define REJECT(b) (!(b))
#define REJECT_ERR(e) ((e) == E_NO_ERROR)

#define REQUIRE(e) do {if (!(e)) return FAIL;} while(0)
#define REQUIRE_NOERR(e) do {if ((e) != E_NO_ERROR) return FAIL;} while(0)

#endif /* TEST_C_API_CMAKEFILES_MACROS_H_ */
