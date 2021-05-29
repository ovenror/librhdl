#ifndef FLATTENCONNECTIONSVISITOR_H
#define FLATTENCONNECTIONSVISITOR_H

#include "interfacedoublevisitor.h"
#include "representation/structural/structural.h"
#include <vector>

namespace rhdl {

class FlattenConnectionsVisitor : public InterfaceDoubleVisitor<true>
{
public:
	FlattenConnectionsVisitor(std::vector<Structural::FlatConnection> &flat_connections, PartIdx from, PartIdx to);

	void visit(const ISingle &i1, const ISingle &i2) override;

private:
	std::vector<Structural::FlatConnection> &flat_connections_;
	PartIdx from_, to_;
};

}

#endif // FLATTENCONNECTIONSVISITOR_H
