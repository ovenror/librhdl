/*
 * existingentityhandle.h
 *
 *  Created on: May 15, 2021
 *      Author: ovenror
 */

#ifndef CONSTRUCTION_EXISTINGENTITYHANDLE_H_
#define CONSTRUCTION_EXISTINGENTITYHANDLE_H_

#include "entityhandle.h"

namespace rhdl {

class ExistingEntityHandle : public EntityHandle {
public:
	ExistingEntityHandle(const std::string &name);
	ExistingEntityHandle(const Entity &entity_);
	virtual ~ExistingEntityHandle();

	const Entity &entity() const override {return entity_;}
	const Interface *interface() const override {return &entity_.interface();}

private:
	virtual void finalize() override;

	const Entity& entity_;
};

} /* namespace rhdl */

#endif /* CONSTRUCTION_EXISTINGENTITYHANDLE_H_ */
