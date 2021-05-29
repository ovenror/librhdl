/*
 * newentityhandle.h
 *
 *  Created on: May 15, 2021
 *      Author: ovenror
 */

#ifndef CONSTRUCTION_NEWENTITYHANDLE_H_
#define CONSTRUCTION_NEWENTITYHANDLE_H_

#include "entityhandle.h"

namespace rhdl {

class NewEntityHandle : public EntityHandle {
public:
	NewEntityHandle(const std::string &name, bool stateless = true);
	NewEntityHandle(std::unique_ptr<Entity> &&entity);
	virtual ~NewEntityHandle();

	const Entity& entity() const override {return *entity_;}
	const Interface *interface() const override {return &placeholder_;}

private:
	virtual void finalize() override;

	std::unique_ptr<Entity> entity_;
	IPlaceholder placeholder_;
};

} /* namespace rhdl */

#endif /* CONSTRUCTION_NEWENTITYHANDLE_H_ */
