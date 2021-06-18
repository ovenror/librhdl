/*
 * interfacemultivisitor.h
 *
 *  Created on: Jun 14, 2021
 *      Author: ovenror
 */

#define SRC_INTERFACE_VISITORS_INTERFACEMULTICONSTVISITOR_H_
#ifndef SRC_INTERFACE_VISITORS_INTERFACEMULTICONSTVISITOR_H_
#define SRC_INTERFACE_VISITORS_INTERFACEMULTICONSTVISITOR_H_

#include "interfacevisitor.h"
#include <cassert>

namespace rhdl {

class InterfaceMultiConstVisitor {
public:
	InterfaceMultiConstVisitor();
	virtual ~InterfaceMultiConstVisitor();

	template <class iterator>
	void go_visit(iterator begin, iterator end);

	void visit(const ISingle &i) override {assert(0);}
	void visit(const IComposite &i) override {assert(0);}
	void visit(const IPlaceholder &i) override {assert(0);}

protected:
	virtual void visit(const ISingle *i) = 0;
	virtual void visit(const IComposite *i) = 0;
	virtual void visit(const IPlaceholder *i) = 0;

private:
	class CollectorBase;
	template <class IFACE> class Collector;
};

template<class iterator>
inline void rhdl::InterfaceMultiConstVisitor::go_visit(iterator begin,
		iterator end)
{
	iterator
}

class InterfaceMultiConstVisitor::CollectorBase : public InterfaceVisitor<true> {
	void visit(const ISingle &i) override {assert(0);}
	void visit(const IComposite &i) override {assert(0);}
	void visit(const IPlaceholder &i) override {assert(0);}
};

template <class IFACE>
class InterfaceMultiConstVisitor::Collector {
	void visit(IFACE &i) override {collected_ = i;}

private:
	IFACE &collected_;
};

} /* namespace rhdl */

#endif /* SRC_INTERFACE_VISITORS_INTERFACEMULTICONSTVISITOR_H_ */
