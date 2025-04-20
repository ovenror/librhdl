/*
 * cvaluecontainer.h
 *
 *  Created on: Apr 8, 2025
 *      Author: js
 */

#ifndef SRC_C_API_CVALUECONTAINER_H_
#define SRC_C_API_CVALUECONTAINER_H_

#include <cassert>

namespace rhdl {

class CValue;
class CObject;

class CValueContainer {
public:
	virtual ~CValueContainer() {}

	virtual const CObject &add(const CValue &) = 0;
	virtual const CObject &add_after_move(const CValue &v) = 0;
};

} /* namespace rhdl */

#endif /* SRC_C_API_CVALUECONTAINER_H_ */
