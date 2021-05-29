/*
 * entityhandle.h
 *
 *  Created on: Apr 3, 2021
 *      Author: ovenror
 */

#ifndef INCLUDE_STRUCTURE_H_
#define INCLUDE_STRUCTURE_H_

#include <rhdl/construction/connectibleentity.h>
#include <rhdl/construction/entityinterface.h>
#include <rhdl/construction/exported.h>
#include <rhdl/construction/interfacible.h>
#include <memory>
#include <string>

namespace rhdl {

class EntityHandle;
class Interface;

class Structure : public Exported {
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

	Structure(std::string name, Mode mode = CREATE_STATELESS);
	~Structure();

	const Connectible &operator>>(const Connectible &to) const {return Exported::operator>>(to);}
	const Connectible &operator<<(const Connectible &from) const {return Exported::operator<<(from);}

	EntityInterface operator[] (const std::string &iname) const;
	void finalize();

protected:
	const Interfacible &interfacible() const override;

private:
	EntityInterface &operator>>(const EntityHandle &);
	EntityInterface &operator<<(const EntityHandle &);
	EntityInterface &operator>>(const EntityInterface &);
	EntityInterface &operator<<(const EntityInterface &);

	std::unique_ptr<EntityHandle> handle_;
};

}

#endif /* INCLUDE_STRUCTURE_H_ */
