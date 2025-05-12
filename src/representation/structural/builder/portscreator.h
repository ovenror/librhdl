/*
 * indexcreator.h
 *
 *  Created on: Jun 1, 2021
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_STRUCTURAL_BUILDER_PORTSCREATOR_H_
#define SRC_REPRESENTATION_STRUCTURAL_BUILDER_PORTSCREATOR_H_

#include "interface/visitors/interfacevisitor.h"
#include "complexport.h"

#include <forward_list>
#include <memory>
#include <unordered_set>

namespace rhdl {

class Interface;

namespace structural::builder {

class Element;
class Port;
class ComplexPort;
class ExistingPort;
class BuilderPort;

class PortsCreator : public InterfaceVisitor<true> {
public:
	PortsCreator(Element &element);
	PortsCreator(BuilderPort &enclosing);

	virtual ~PortsCreator();

	void visit(const ISingle &i) override;
	void visit(const IComposite &i) override;

	std::unique_ptr<ExistingPort> create(const Interface &i);
	ComplexPort create(const IComposite &i, const std::string *name = nullptr);
	ComplexPort::Enclosed createEnclosed(const IComposite &i);

private:
	void createEnclosed(
			const IComposite &i,
			ComplexPort::Enclosed &set);

	Element &element_;
	ComplexPort::Enclosed *enclosed_ = nullptr;
};

} /* namespace structural::builder */
} /* namespace rhdl */

#endif /* SRC_REPRESENTATION_STRUCTURAL_BUILDER_PORTSCREATOR_H_ */
