#ifndef FLATTENVISITOR_H
#define FLATTENVISITOR_H

#include "interfacevisitor.h"
#include <vector>
#include <cassert>
#include <tuple>

namespace rhdl {

class Flattener : public InterfaceVisitor<true>
{
public:
	Flattener(std::vector<const ISingle *> &flat);

	void visit(const ISingle &i) override;
	void visit(const IComposite &i) override;

private:
	std::vector<const ISingle *> &flat_;
};

}
#endif // FLATTENVISITOR_H
