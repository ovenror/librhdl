#ifndef FLATTENCONNECTIONSVISITOR_H
#define FLATTENCONNECTIONSVISITOR_H

#include "interfacedoublevisitor.h"
#include "representation/structural/structural.h"
#include <vector>

namespace rhdl {

class ConnectionFlattener : public InterfaceDoubleVisitor<true>
{
public:
	ConnectionFlattener(std::vector<Structural::FlatConnection> &flat_connections, PartIdx from, PartIdx to);

	void visit(const ISingle &i1, const ISingle &i2) override;

private:
	std::vector<Structural::FlatConnection> &flat_connections_;
	PartIdx from_, to_;
};

}

#endif // FLATTENCONNECTIONSVISITOR_H
