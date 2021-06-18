/*
 * port.h
 *
 *  Created on: May 28, 2022
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_STRUCTURAL_PORT_H_
#define SRC_REPRESENTATION_STRUCTURAL_PORT_H_

#include "element.h"

#include <ostream>

namespace rhdl {

class ISingle;

namespace structural {

struct Port  {
	Port(ElementIdx e, const ISingle &i);

	bool isExternal() const {return element_ == structureIdx;}
	ElementIdx element() const {return element_;}
	const ISingle &iface() const {return iface_;}
	bool needs_closing() const {return needs_closing_;}

	void close() {needs_closing_ = true;}

private:
	ElementIdx element_;
	const ISingle &iface_;
	bool needs_closing_ = false;
};

std::ostream &operator<<(std::ostream &os, const Port &p);

} /* namespace structural */
} /* namespace rhdl */

#endif /* SRC_REPRESENTATION_STRUCTURAL_PORT_H_ */
