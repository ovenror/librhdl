/*
 * pert.h
 *
 *  Created on: Jun 20, 2021
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_STRUCTURAL_BUILDER_PART_H_
#define SRC_REPRESENTATION_STRUCTURAL_BUILDER_PART_H_

#include "element.h"
#include "complexport.h"

#include "util/partitionable.h"

namespace rhdl {

class Entity;

namespace structural::builder {

class StructureBuilder;

class Part : public Element, public Partitionable<Part, StructureBuilder> {
public:
	Part(const Entity& entity);
	Part(Part &&) = default;

	virtual ~Part();

	PortContainer &top() override {return top_;}
	ComplexPort &topPort() {return top_;}
	StructureBuilder &builder() override {assert(pcOwner()); return *pcOwner();}
	bool isTheStructure() const override {return false;}
	const Entity &entity() const {return entity_;}

protected:
	void toStream(std::ostream &os) const override;

private:
	template <class> friend class PartitionClassBase;

	const Entity &entity_;
	ComplexPort top_;
};

ComplexPort &makeComponent(const std::string &entityName);

} /* namespace structural::builder */
} /* namespace rhdl */

#endif /* SRC_REPRESENTATION_STRUCTURAL_BUILDER_PART_H_ */
