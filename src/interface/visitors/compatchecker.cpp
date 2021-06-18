/*
 * compatbase.cpp
 *
 *  Created on: Nov 6, 2021
 *      Author: ovenror
 */

#include "compatchecker.h"

#include "../cresult/csingle.h"
#include "../cresult/ccomposite.h"
#include "../cresult/cdifftypes.h"

namespace rhdl {

template<class RESULT>
void CompatChecker<RESULT>::visit(const ISingle &i1, const ISingle &i2)
{
	result_ = computer_.template compute<CSingle>(i1, i2);
}

template void CompatChecker<bool>::visit(const ISingle &, const ISingle &);
template void CompatChecker<Interface::CResult>::visit(const ISingle &, const ISingle &);

template<class RESULT>
void CompatChecker<RESULT>::visit(
		const IComposite &i1, const IComposite &i2)
{
	result_ = computer_.template compute<CComposite>(i1, i2);
}

template void CompatChecker<bool>::visit(const IComposite &, const IComposite &);
template void CompatChecker<Interface::CResult>::visit(const IComposite &, const IComposite &);

template<class RESULT>
void CompatChecker<RESULT>::visit(
		const Interface &i1, const Interface &i2)
{
	result_ = computer_.template compute<CDiffTypes>(i1, i2);
}

template void CompatChecker<bool>::visit(const Interface &, const Interface &);
template void CompatChecker<Interface::CResult>::visit(const Interface &, const Interface &);

} /* namespace rhdl */
