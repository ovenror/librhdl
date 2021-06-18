/*
 * structureconnector.h
 *
 *  Created on: Jul 12, 2021
 *      Author: ovenror
 */

#ifndef INCLUDE_RHDL_CONSTRUCTION_STRUCTURECONNECTOR_H_
#define INCLUDE_RHDL_CONSTRUCTION_STRUCTURECONNECTOR_H_

#include <rhdl/construction/connector.h>

namespace rhdl {

class Structure;

class StructureConnector: public Connector {
public:
	StructureConnector(structural::builder::Port &thePort);

	StructureConnector operator [](const std::string &iname) const;

	const Connectible &operator>>(const Connectible & c) const {return Connectible::operator>>(c);}
	const Connectible &operator<<(const Connectible &c) const {return Connectible::operator<<(c);}
	Connectible &operator=(const Connectible &c) {return Connectible::operator=(c);}

private:
	const Structure &operator>>(const Structure &) const;
	const Structure &operator<<(const Structure &) const;
	Structure &operator=(const Structure &);
	const StructureConnector &operator>>(const StructureConnector &) const;
	const StructureConnector &operator<<(const StructureConnector &) const;
	StructureConnector &operator=(const StructureConnector &);
};

} /* namespace rhdl */

#endif /* INCLUDE_RHDL_CONSTRUCTION_STRUCTURECONNECTOR_H_ */
