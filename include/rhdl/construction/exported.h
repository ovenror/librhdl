/*
 * exported.h
 *
 *  Created on: Apr 4, 2021
 *      Author: ovenror
 */

#ifndef INCLUDE_RHDL_CONSTRUCTION_EXPORTED_H_
#define INCLUDE_RHDL_CONSTRUCTION_EXPORTED_H_

#include <rhdl/construction/interfacible.h>
#include <string>
#include <memory>
#include <tuple>

namespace rhdl {

class EntityHandle;
class PartHandle;
class Interface;

class Exported : public Connectible {
public:
	std::pair<const Interface *, const Interface *> find_connectible_interfaces(const Connectible &to, const ConnectionPredicate &predicate) const override;
	std::pair<const Interface *, const Interface *> find_connectible_interfaces(const Interfacible &to, const ConnectionPredicate &predicate) const override;
	const Interface *find_connectible_interface(const Connectible &to, const ConnectionPredicate &predicate) const override;

	const Interface *interface() const override;

protected:
	Exported();
};

}


#endif /* INCLUDE_RHDL_CONSTRUCTION_EXPORTED_H_ */
