/*
 * compatbase.h
 *
 *  Created on: Nov 6, 2021
 *      Author: ovenror
 */

#ifndef SRC_INTERFACE_VISITORS_COMPATCHECKER_H_
#define SRC_INTERFACE_VISITORS_COMPATCHECKER_H_

#include "interfacedoublevisitor.h"
#include "../interface.h"
#include "construction/connectionpredicate.h"
#include <type_traits>

namespace rhdl {

class CTrue;

template <class RESULT>
class CompatChecker;

class CompatCheckerComputerBase {
public:
	CompatCheckerComputerBase(const ConnectionPredicate &predicate)
		: predicate_(predicate)
	{}

protected:
	const ConnectionPredicate predicate_;
};

template <class RESULT>
struct CompatCheckerComputer;

template <>
struct CompatCheckerComputer<Interface::CResult> : public CompatCheckerComputerBase {
	using CompatCheckerComputerBase::CompatCheckerComputerBase;

	template <class CRESULT, class LHS, class RHS>
	Interface::CResult compute(const LHS &lhs, const RHS &rhs) const
	{
		return std::make_unique<CRESULT>(lhs, rhs, predicate_);
	}
};


template <>
struct CompatCheckerComputer<bool> : public CompatCheckerComputerBase {
	using CompatCheckerComputerBase::CompatCheckerComputerBase;

	template <class CRESULT, class LHS, class RHS>
	bool compute(const LHS &lhs, const RHS &rhs) const
	{
		return CRESULT::compatible(lhs, rhs, predicate_);
	}
};


template <class RESULT>
class CompatChecker : public InterfaceDoubleVisitor<true> {
public:
	using Computer = CompatCheckerComputer<RESULT>;

	CompatChecker(const ConnectionPredicate &predicate)	: computer_(predicate) {}
	virtual ~CompatChecker() {}

	void visit(const ISingle &i1, const ISingle &i2) override;
	void visit(const IComposite &i1, const IComposite &i2) override;
	void visit(const Interface &i1, const Interface &i2) override;

	RESULT result() {return std::move(result_);}

private:
	const Computer computer_;
	RESULT result_;
};

} /* namespace rhdl */

#endif /* SRC_INTERFACE_VISITORS_COMPATCHECKER_H_ */
