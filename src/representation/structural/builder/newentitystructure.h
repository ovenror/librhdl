/*
 * newentitystructure.h
 *
 *  Created on: Jul 26, 2021
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_STRUCTURAL_BUILDER_NEWENTITYSTRUCTURE_H_
#define SRC_REPRESENTATION_STRUCTURAL_BUILDER_NEWENTITYSTRUCTURE_H_

#include "structure.h"
#include "builderport.h"

#include <string>

namespace rhdl {
namespace structural {
namespace builder {

class BuilderPort;

class NewEntityStructure : public Structure {
public:
	NewEntityStructure(Namespace &ns, std::string entityName, bool stateless);
	virtual ~NewEntityStructure();

	PortContainer &top() override {return *top_;}
	void finalize() override;

	void replaceTopBuilder(std::unique_ptr<ComplexPort> &&) override;

protected:
	const std::string &name() const override {return entityName_;}

private:
	const Entity &entity() const override;
	void doFinalize();

	std::string entityName_;
	std::unique_ptr<PortContainer> top_ = nullptr;
	BuilderPort *topBuilder = nullptr;
	Namespace &ns_;
};

} /* namespace builder */
} /* namespace structural */
} /* namespace rhdl */

#endif /* SRC_REPRESENTATION_STRUCTURAL_BUILDER_NEWENTITYSTRUCTURE_H_ */
