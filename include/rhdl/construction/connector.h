/*
 * port.h
 *
 *  Created on: Jul 12, 2021
 *      Author: ovenror
 */

#ifndef SRC_CONSTRUCTION_PORT_H_
#define SRC_CONSTRUCTION_PORT_H_

#include <rhdl/construction/connectible.h>
#include <rhdl/construction/porthandle.h>

namespace rhdl {

namespace structural::builder { class ExistingPort; }

class Connector : public Connectible {
public:
	Connector(structural::builder::Port &thePort);
	Connector(const Connector &);

	~Connector();

	Connector operator[] (const std::string &iname) const;
	Connectible &operator=(const Connector &c) {return Connectible::operator=(c);}

protected:
	structural::builder::Port &port() const final override {return ph_.port();}
	PortHandle ph_;
};

} /* namespace rhdl */

#endif /* SRC_CONSTRUCTION_PORT_H_ */
