/*
 * cvaluecontainer.h
 *
 *  Created on: Apr 8, 2025
 *      Author: js
 */

#ifndef SRC_C_API_CVALUECONTAINER_H_
#define SRC_C_API_CVALUECONTAINER_H_

#include "cobject.h"

namespace rhdl {

class CValue;

class CValueContainer : public CObject {
public:
	CValueContainer(rhdl_type typeId, std::string name);
	CValueContainer(CValueContainer &&moved);

	virtual ~CValueContainer();

	virtual const CObject &add(const CValue &) = 0;
	virtual const CObject &add_after_move(const CValue &) = 0;
};

} /* namespace rhdl */

#endif /* SRC_C_API_CVALUECONTAINER_H_ */
