/*
 * index.h
 *
 *  Created on: Jun 13, 2021
 *      Author: ovenror
 */

#ifndef SRC_REPRESENTATION_STRUCTURAL_BUILDER_ELEMENT_H_
#define SRC_REPRESENTATION_STRUCTURAL_BUILDER_ELEMENT_H_

#include "interface/interface.h"

#include <iostream>
#include <memory>
#include <unordered_map>

namespace rhdl {

class Interface;
class IComposite;
class Entity;
class Connectible;

namespace structural::builder {

class Structure;
class Part;
class StructureBuilder;
class PortContainer;
class Port;
class BuilderPort;
class ExistingPort;
class ComplexPort;

class Element {
public:
	virtual ~Element();

	Port &topPort();

	virtual PortContainer &top() = 0;

	virtual void replaceTopBuilder(std::unique_ptr<ComplexPort> &&)
		{assert(0);}

	void invalidate();

	virtual bool isTheStructure() const = 0;

	virtual StructureBuilder &builder() = 0;

	static void combineBuilders(Element &lhs, Element &rhs);

	static ConnectionPredicate predicate(
			const Element &from, const Element &to, bool directional = true);

protected:
	friend std::ostream &operator<<(std::ostream &os, const Element &element);
	void initIdxs(const IComposite &iface);
	virtual void toStream(std::ostream &os) const = 0;
};

std::ostream &operator<<(std::ostream &os, const Element &element);

} /* namespace structural::builder */
} /* namespace rhdl */

#endif /* SRC_REPRESENTATION_STRUCTURAL_BUILDER_ELEMENT_H_ */
