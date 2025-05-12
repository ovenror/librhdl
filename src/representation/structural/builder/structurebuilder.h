#ifndef NET_H
#define NET_H

#include "part.h"

#include "../element.h"

#include "util/owningpartitionclass.h"

#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <set>
#include <vector>

namespace rhdl {

class Entity;
class Interface;
class ISingle;

namespace structural {

class Connection;

namespace builder {

class Element;
class Part;
class Port;
class CompleteStructureBuilder;
class PartialStructureBuilder;

class StructureBuilder
{
public:
	StructureBuilder(bool stateless = true);
	virtual ~StructureBuilder();

	ComplexPort &add(const Entity &partEntity, const std::string *name = nullptr);

	bool stateless() {return stateless_;}

	static void combineIffNotSame(StructureBuilder &b1, StructureBuilder &b2);
	virtual void combineWith(StructureBuilder &b) = 0;
	virtual void combineWith(CompleteStructureBuilder &s) = 0;

	virtual void ingest(PartialStructureBuilder &victim);

	void onAssign(Part &p) {assertStatelessness(p);}
	void onReassign(Part &p) {assertStatelessness(p);}

	void release(PartitionClassBase<Part> &);

	void invalidate();

protected:
	friend std::ostream &operator<<(std::ostream &os, StructureBuilder &net);

	std::map<Element *, ElementIdx> buildParts(
			std::vector<const Entity *> &target) const;

	std::vector<structural::Connection>
		buildConnections(
			const std::map<Element *, ElementIdx> &elementMap) const;

	const OwningPartitionClass<Part> &parts() const {return parts_;}

private:
	void assertStatelessness(const Part &);
	virtual void setStateful() {stateless_ = false;}

	OwningPartitionClass<Part> parts_;

	bool stateless_ = true;
};

std::ostream &operator<<(std::ostream &os, StructureBuilder &net);

}}}

#endif // NET_H
