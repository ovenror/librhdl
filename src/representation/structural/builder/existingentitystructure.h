/*
 * existingentitystructure.h
 *
 *  Created on: Jul 27, 2021
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_STRUCTURAL_BUILDER_EXISTINGENTITYSTRUCTURE_H_
#define SRC_REPRESENTATION_STRUCTURAL_BUILDER_EXISTINGENTITYSTRUCTURE_H_

#include "structure.h"
#include "complexport.h"

namespace rhdl {
namespace structural {
namespace builder {

class ExistingEntityStructure : public Structure {
public:
	ExistingEntityStructure(const std::string &entityName);
	virtual ~ExistingEntityStructure();

	PortContainer &top() override {return top_;}

protected:
	const std::string &name() const override {return entity_.name();}

private:
	ExistingEntityStructure(const Entity &entity);

	const Entity &entity() const override {return entity_;}

	const Entity &entity_;
	ComplexPort top_;
};

} /* namespace builder */
} /* namespace structural */
} /* namespace rhdl */

#endif /* SRC_REPRESENTATION_STRUCTURAL_BUILDER_EXISTINGENTITYSTRUCTURE_H_ */
