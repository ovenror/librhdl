/*
 * RootNamespace.h
 *
 *  Created on: Oct 7, 2024
 *      Author: js
 */

#ifndef SRC_C_API_ROOTNAMESPACE_H_
#define SRC_C_API_ROOTNAMESPACE_H_

#include "namespace.h"

namespace rhdl {

class RootNamespace : public Namespace {
public:
	RootNamespace();
	virtual ~RootNamespace();

	static void init();
};

extern RootNamespace rootNamespace;

}

#endif /* SRC_C_API_ROOTNAMESPACE_H_ */
