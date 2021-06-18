/*
 * cbase.h
 *
 *  Created on: Oct 6, 2021
 *      Author: ovenror
 */

#ifndef SRC_INTERFACE_CRESULT_CBASE_H_
#define SRC_INTERFACE_CRESULT_CBASE_H_

#include "compatibilityresult.h"
#include "../predicate.h"

namespace rhdl {

template <class LHS, class RHS, class DERIVED>
class CBaseOpsBase {
	template <class RESULT, class dummy>
	struct GetSubResultType;

	template <class PARAM, class dummy>
	struct GetStored {
		using type = const PARAM;
	};

	template <class REF, class dummy>
	struct GetStored<REF&, dummy> {
		using type = const REF &;
	};

public:
	using Super = CBaseOpsBase;
	using This = DERIVED;
	using rLHS = const typename std::remove_reference<LHS>::type &;
	using rRHS = const typename std::remove_reference<RHS>::type &;

	CBaseOpsBase(rLHS, rRHS, const ConnectionPredicate &predicate);

	typename GetStored<LHS, None>::type lhs;
	typename GetStored<RHS, None>::type rhs;
	const ConnectionPredicate predicate;

	template <class RESULT>
	void eval_common(RESULT &result) const {}

	bool compatible();

protected:
	template <class RESULT>
	using SubType = typename GetSubResultType<RESULT, None>::type;

	template <class RESULT>
	static bool checkSub(const RESULT &sub) {return sub -> success();}
	static bool checkSub(bool &sub) {return sub;}

	template <class... Args>
	void issue(
			Errorcode (DERIVED::*ISSUE_FUNC)(std::ostream &, Args...) const,
			bool &r, Args&&...) const;

	template <class RESULT, class... Args>
	void issue(
			Errorcode (DERIVED::*ISSUE_FUNC)(std::ostream &, Args...) const,
			const RESULT &r, Args&&...) const;

	template <class SRESULT, class... Args>
	static void use(bool &, Args&&...);

	template <class SRESULT, class RESULT, class... Args>
	static void use(const RESULT &, Args&&...);

	static Interface::CResult smartmove(Interface::CResult &r) {return std::move(r);}
	static Interface::CResult smartmove(bool &r) {return Interface::CResult();}

private:
	template <class RESULT, class dummy>
	struct GetSubResultType {
		using type = Interface::CResult;
	};

	template <class dummy>
	struct GetSubResultType<bool, dummy> {
		using type = bool;
	};
};

template <class OPS>
class CBase;

template <class T, class DERIVED>
using CSametypeOpsBase = CBaseOpsBase<T, T, DERIVED>;

template <class OPS>
class CBase : public CompatibilityResult {
	using rLHS = typename OPS::rLHS;
	using rRHS = typename OPS::rRHS;

public:
	using Predicate = ConnectionPredicate;
	using Super = CBase;

	CBase(rLHS from, rRHS to, const Predicate &predicate);
	virtual ~CBase() {}

	static bool compatible(rLHS, rRHS, const ConnectionPredicate &);
	void eval_int() const override;

protected:
	template <class, class, class> friend class CBaseOpsBase;

	template <class... Args>
	void issue(Errorcode (OPS::*ISSUE_FUNC)(std::ostream &, Args...) const, Args&&...) const;

	const OPS ops_;
};

template<class OPS>
inline CBase<OPS>::CBase(rLHS lhs, rRHS rhs, const Predicate &predicate)
	: ops_(OPS(lhs, rhs, predicate))
{}

template<class LHS, class RHS, class DERIVED>
inline CBaseOpsBase<LHS, RHS, DERIVED>::CBaseOpsBase(
		rLHS l, rRHS r, const ConnectionPredicate &p) :
	lhs(l), rhs(r), predicate(p)
{}

template<class OPS>
inline bool CBase<OPS>::compatible(
		rLHS lhs, rRHS rhs, const ConnectionPredicate &predicate)
{
	return OPS(lhs, rhs, predicate).compatible();
}

template<class OPS>
inline void CBase<OPS>::eval_int() const
{
	ops_.eval_common(*this);
}

template <class LHS, class RHS, class DERIVED>
template<class RESULT, class... Args>
inline void CBaseOpsBase<LHS,RHS,DERIVED>::issue(
		Errorcode (DERIVED::*ISSUE_FUNC)(std::ostream&, Args...) const,
		const RESULT& r, Args&&... args) const
{
	r.issue(ISSUE_FUNC, std::forward<Args>(args)...);
}

template <class LHS, class RHS, class DERIVED>
template<class... Args>
inline void CBaseOpsBase<LHS,RHS,DERIVED>::issue(
		Errorcode (DERIVED::*ISSUE_FUNC)(std::ostream&, Args...) const,
		bool& r, Args&&... args) const
{
	r = false;
}


template <class OPS>
template <class... Args>
inline void CBase<OPS>::issue(
		Errorcode (OPS::*ISSUE_FUNC)(std::ostream &, Args...) const,
		Args&&... args) const
{
	ec_ = (ops_.*ISSUE_FUNC)(msg_, std::forward<Args>(args)...);
	success_ = false;
}

template <class LHS, class RHS, class DERIVED>
template<class SRESULT, class RESULT, class... Args>
inline void CBaseOpsBase<LHS, RHS, DERIVED>::use(
		const RESULT &r, Args&&... args)
{
	r.use(SRESULT(std::forward<Args>(args)...));
}

template <class LHS, class RHS, class DERIVED>
template<class SRESULT, class... Args>
inline void CBaseOpsBase<LHS, RHS, DERIVED>::use(bool &r, Args&&... args)
{
	r = SRESULT::compatible(std::forward<Args>(args)...);
}

template<class LHS, class RHS, class DERIVED>
inline bool CBaseOpsBase<LHS, RHS, DERIVED>::compatible()
{
	bool result;
	eval_common(result);
	return result;
}


} /* namespace rhdl */

#endif /* SRC_INTERFACE_CRESULT_CBASE_H_ */
