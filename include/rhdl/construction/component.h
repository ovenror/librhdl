/*
 * Component.h
 *
 *  Created on: Apr 4, 2021
 *      Author: ovenror
 */

#ifndef INCLUDE_RHDL_CONSTRUCTION_COMPONENT_H_
#define INCLUDE_RHDL_CONSTRUCTION_COMPONENT_H_

#include <rhdl/construction/connector.h>

namespace rhdl {

class Component : public Connector {
public:
	Component(const std::string &name);

	Connectible &operator=(const Component &c) {return Connectible::operator=(c);}
};

}

#endif /* INCLUDE_RHDL_CONSTRUCTION_COMPONENT_H_ */
