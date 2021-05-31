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
	Flattener(std::vector<const ISingle *> &flat_interface);

	void visit(const ISingle &i) override;
	void visit(const IComposite &i) override;
	void visit(const IPlaceholder &i) override {std::ignore=i;assert(0);}

private:
	std::vector<const ISingle *> &flat_interface_;
};

}
#endif // FLATTENVISITOR_H
