/*
 * entityhandle.h
 *
 *  Created on: Apr 3, 2021
 *      Author: ovenror
 */

#ifndef INCLUDE_STRUCTURE_H_
#define INCLUDE_STRUCTURE_H_

#include <rhdl/construction/connectible.h>
#include <rhdl/construction/structureconnector.h>

#include <memory>
#include <string>

namespace rhdl {

class Entity;

namespace structural { namespace builder { class Structure; }}

class Structure : public Connectible {
private:
	struct Hidden {
		#include <rhdl/construction/c/flags.h>
	};
public:
	enum Mode {
		CREATE_STATELESS = static_cast<int>(Hidden::F_CREATE_STATELESS),
		CREATE_STATEFUL = static_cast<int>(Hidden::F_CREATE_STATEFUL),
		EXISTS = static_cast<int>(Hidden::F_EXISTS)
	};

	Structure(const std::string &name, Mode mode = CREATE_STATELESS);
	~Structure();

	StructureConnector operator[] (const std::string &iname) const;

	const Connectible &operator>>(const Connectible & c) const {return Connectible::operator>>(c);}
	const Connectible &operator<<(const Connectible &c) const {return Connectible::operator<<(c);}
	Connectible &operator=(const Connectible &c) {return Connectible::operator=(c);}

	void finalize();
	void abort();

protected:
	std::unique_ptr<structural::builder::Structure> impl_;

private:
	const Structure &operator>>(const Structure &) const;
	const Structure &operator<<(const Structure &) const;
	Structure &operator=(const Structure &);
	const StructureConnector &operator>>(const StructureConnector &) const;
	const StructureConnector &operator<<(const StructureConnector &) const;
	StructureConnector &operator=(const StructureConnector &);

	structural::builder::Port &port() const final override;
};

}

#endif /* INCLUDE_STRUCTURE_H_ */
