/*
 * representationtype.h
 *
 *  Created on: Jun 4, 2023
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_REPRESENTATIONTYPE_H_
#define SRC_REPRESENTATION_REPRESENTATIONTYPE_H_

#include "representationtypeid.h"
#include <string>

namespace rhdl {

class RepresentationType {
public:
	using ID = RepresentationTypeID;

	RepresentationType(ID id, std::string name);
	~RepresentationType();

	const std::string &name() const {return name_;}

private:
	const ID id_;
	const std::string name_;
};

} /* namespace rhdl */

#endif /* SRC_REPRESENTATION_REPRESENTATIONTYPE_H_ */
