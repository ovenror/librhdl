/*
 * Component.h
 *
 *  Created on: Apr 4, 2021
 *      Author: ovenror
 */

#ifndef INCLUDE_RHDL_CONSTRUCTION_COMPONENT_H_
#define INCLUDE_RHDL_CONSTRUCTION_COMPONENT_H_

#include <rhdl/construction/exported.h>
#include <rhdl/construction/partinterface.h>
#include <memory>
#include <string>

namespace rhdl {

class PartHandle;

class Component : public Exported {
public:
	Component(const std::string &name);
	~Component();

	PartInterface operator[] (const std::string &iname) const;

protected:
	const Interfacible &interfacible() const override;

private:
	std::unique_ptr<PartHandle> handle_;
};

}

#endif /* INCLUDE_RHDL_CONSTRUCTION_COMPONENT_H_ */
